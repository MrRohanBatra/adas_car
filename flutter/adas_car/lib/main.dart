import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:adas_car/settings.dart';
import 'package:http/http.dart' as http;

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: ThemeData(
        useMaterial3: true,
        colorScheme: const ColorScheme.light(
          primary: Colors.blueAccent,
          surface: Colors.white,
          onSurface: Colors.black,
        ),
      ),
      darkTheme: ThemeData(
        useMaterial3: true,
        colorScheme: const ColorScheme.dark(
          primary: Colors.blueAccent,
          surface: Colors.black87,
          onSurface: Colors.white,
        ),
      ),
      themeMode: ThemeMode.system,
      home: const CarControl(),
    );
  }
}

class CarControl extends StatefulWidget {
  const CarControl({super.key});

  @override
  State<CarControl> createState() => _CarControlState();
}

class _CarControlState extends State<CarControl> {
  int sel = 0; // 0 = Home, 1 = Settings
  bool isOn = false;
  String ipaddress = "192.168.4.1";

  @override
  void initState() {
    super.initState();
    _loadIpAddress();
  }

  Future<void> _loadIpAddress() async {
    SharedPreferences prefs = await SharedPreferences.getInstance();
    setState(() {
      ipaddress = prefs.getString('ip_address') ?? "192.168.4.1";
    });
  }

  Future<void> _updateIpAddress(String newIp) async {
    SharedPreferences prefs = await SharedPreferences.getInstance();
    await prefs.setString('ip_address', newIp);
    setState(() {
      ipaddress = newIp;
    });
  }

  bool ign = false;
  Future<void> sendCommand(String command) async {
    ScaffoldMessenger.of(context).removeCurrentSnackBar();
    debugPrint("function called with command: $command");
    String address = "http://$ipaddress/command";
    final url = Uri.parse(address);
    try {
      var response = await http.post(
        url,
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: {"cmd": command},
      );
      if (response.statusCode == 200) {
        debugPrint("command executed: $command");
      } else if (response.statusCode == 300) {
        debugPrint(response.body.trim());
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text(response.body.trim()),
            backgroundColor: Colors.amberAccent,
            duration: Duration(milliseconds: 200),
          ),
        );
      }
    } catch (e) {
      debugPrint("Error: $e");
    }
  }

  bool mode = true;
  Future<void> handleMode() async {
    ScaffoldMessenger.of(context).removeCurrentSnackBar();
    String url = "http://$ipaddress/mode";
    final uri = Uri.parse(url);
    String modeType = mode ? "manual" : "auto"; // Determine mode
    try {
      final response = await http.post(
        uri,
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: {"mode": modeType},
      );

      if (response.statusCode == 200) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text("Switched to mode ${response.body.trim()}"),
            backgroundColor: Colors.greenAccent,
            duration: Duration(milliseconds: 600),
          ),
        );
        mode=!mode;
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text("Failed to switch mode"),
            backgroundColor: Colors.redAccent,
            duration: Duration(milliseconds: 600),
          ),
        );
      }
    } catch (e) {
      debugPrint("Error: $e");
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text("Error: $e"),
          backgroundColor: Colors.redAccent,
        ),
      );
    }
  }

  void power() async {
    try {
      debugPrint("Function called");
      String url = "http://$ipaddress/ign";
      final uri = Uri.parse(url);

      final response = await http.post(
        uri,
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body:
            "ign=${Uri.encodeQueryComponent((!isOn).toString())}", // Fix: Correct encoding
      );

      if (response.statusCode == 200) {
        debugPrint("IGN Changed to ${!isOn}");
        if (mounted) {
          setState(() {
            isOn = !isOn;
          });
        }
        ScaffoldMessenger.of(context).removeCurrentSnackBar();
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text("Ignition ${isOn ? "ON" : "OFF"}"),
            duration: Duration(milliseconds: 200),
            backgroundColor: Colors.greenAccent,
          ),
        );
      } else {
        debugPrint(
          "Unexpected response: ${response.statusCode} - ${response.body}",
        );
        ScaffoldMessenger.of(context).removeCurrentSnackBar();
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text(
              "Unexpected response: ${response.statusCode} - ${response.body}",
            ),
            duration: Duration(milliseconds: 200),
            backgroundColor: Colors.redAccent,
          ),
        );
      }
    } catch (e) {
      debugPrint("Error: $e");
      ScaffoldMessenger.of(context).removeCurrentSnackBar();
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text("error"),
          duration: Duration(milliseconds: 200),
          backgroundColor: Colors.redAccent,
        ),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context).colorScheme;

    return Scaffold(
      backgroundColor: theme.surface,
      body: SafeArea(
        top: true,
        bottom: false,
        child: IndexedStack(
          index: sel,
          children: [
            _buildHomePage(),
            Settings(ipaddress: ipaddress, onIpChanged: _updateIpAddress),
          ],
        ),
      ),
      bottomNavigationBar: BottomNavigationBar(
        currentIndex: sel,
        backgroundColor: theme.surface,
        selectedItemColor: theme.primary,
        unselectedItemColor: theme.onSurface.withOpacity(0.6),
        onTap: (int index) {
          setState(() {
            sel = index;
          });
        },
        items: const [
          BottomNavigationBarItem(icon: Icon(Icons.home), label: 'Home'),
          BottomNavigationBarItem(
            icon: Icon(Icons.settings),
            label: 'Settings',
          ),
        ],
      ),
    );
  }

  Widget _buildHomePage() {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          GestureDetector(
            onTap: power,
            child: Container(
              decoration: BoxDecoration(
                shape: BoxShape.circle,
                color: isOn ? Colors.green : Colors.red,
                boxShadow: const [
                  BoxShadow(
                    color: Colors.black26,
                    blurRadius: 10,
                    spreadRadius: 2,
                    offset: Offset(0, 5),
                  ),
                ],
              ),
              padding: const EdgeInsets.all(30),
              child: const Icon(
                Icons.power_settings_new,
                size: 60,
                color: Colors.white,
              ),
            ),
          ),
          const SizedBox(height: 40),
          Opacity(
            opacity: isOn ? 1.0 : 0.5,
            child: Column(
              children: [
                GestureDetector(
                  onTapDown:
                      (_) => sendCommand("F"), // Send command when pressed
                  onTapUp: (_) => sendCommand("S"), // Stop when released
                  onTapCancel:
                      () => sendCommand("S"), // Stop if finger slides away
                  child: Icon(Icons.arrow_upward, size: 60),
                ),
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    GestureDetector(
                      onTapDown:
                          (_) => sendCommand("L"), // Send command when pressed
                      onTapUp: (_) => sendCommand("S"), // Stop when released
                      onTapCancel:
                          () => sendCommand("S"), // Stop if finger slides away
                      child: Icon(Icons.arrow_back, size: 60),
                    ),

                    const SizedBox(width: 50),
                    GestureDetector(
                      onTapDown:
                          (_) => sendCommand("R"), // Send command when pressed
                      onTapUp: (_) => sendCommand("S"), // Stop when released
                      onTapCancel:
                          () => sendCommand("S"), // Stop if finger slides away
                      child: Icon(Icons.arrow_forward, size: 60),
                    ),
                  ],
                ),
                GestureDetector(
                  onTapDown:
                      (_) => sendCommand("R"), // Send command when pressed
                  onTapUp: (_) => sendCommand("S"), // Stop when released
                  onTapCancel:
                      () => sendCommand("S"), // Stop if finger slides away
                  child: Icon(Icons.arrow_downward, size: 60),
                ),
                const SizedBox(height: 20),
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    ElevatedButton.icon(
                      onPressed: isOn ? () => sendCommand("B") : null,
                      icon: Icon(Icons.autorenew_rounded, size: 50),
                      label:Text("G Turn"),
                    ),
                    const SizedBox(width: 20),
                    ElevatedButton.icon(
                      onPressed: isOn ? () => handleMode(): null,
                      icon: Icon(Icons.settings, size: 50),
                      label:Text("Mode"),
                    ),
                  ],
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
