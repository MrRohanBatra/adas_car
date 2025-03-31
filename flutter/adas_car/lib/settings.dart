// import 'package:flutter/material.dart';
//
// class Settings extends StatefulWidget {
//   final String ipaddress;
//
//   const Settings({super.key, required this.ipaddress});
//
//   @override
//   _SettingsState createState() => _SettingsState();
// }
//
// class _SettingsState extends State<Settings> {
//   late TextEditingController _ipController;
//
//   @override
//   void initState() {
//     super.initState();
//     _ipController = TextEditingController(text: widget.ipaddress); // Use passed IP
//   }
//
//   @override
//   void dispose() {
//     _ipController.dispose();
//     super.dispose();
//   }
//
//   @override
//   Widget build(BuildContext context) {
//     return Scaffold(
//       appBar: AppBar(title: const Text("Settings")),
//       body: SafeArea(
//         child: Padding(
//           padding: const EdgeInsets.all(16.0),
//           child: Column(
//             crossAxisAlignment: CrossAxisAlignment.start,
//             children: [
//               const Text(
//                 "Project Settings",
//                 style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
//               ),
//               const SizedBox(height: 20),
//
//               // IP Address Input Row
//               Row(
//                 children: [
//                   const Text(
//                     "IP : ",
//                     style: TextStyle(fontWeight: FontWeight.bold,fontSize: 20),
//                   ),
//                   const SizedBox(width: 10),
//                   Expanded(
//                     child: TextField(
//                       controller: _ipController,
//                       decoration: const InputDecoration(
//                         border: OutlineInputBorder(),
//                         prefixIcon: Icon(Icons.router),
//                         contentPadding: EdgeInsets.symmetric(vertical: 10),
//                       ),
//                       keyboardType: TextInputType.text,
//                     ),
//                   ),
//                 ],
//               ),
//
//               const SizedBox(height: 20),
//
//               // Save Button
//               Center(
//                 child: ElevatedButton(
//                   onPressed: () {
//                     String newIp = _ipController.text;
//
//                     if (newIp.isNotEmpty) {
//                       ScaffoldMessenger.of(context).showSnackBar(
//                         SnackBar(content: Text("IP Address Saved: $newIp")),
//                       );
//                     }
//
//                     // Pass the updated IP back to the previous screen
//                     Navigator.pop(context, newIp);
//                   },
//                   child: const Text("Save"),
//                 ),
//               ),
//             ],
//           ),
//         ),
//       ),
//     );
//   }
// }
import 'package:flutter/material.dart';

class Settings extends StatefulWidget {
  final String ipaddress;
  final ValueChanged<String> onIpChanged;

  const Settings({super.key, required this.ipaddress, required this.onIpChanged});

  @override
  _SettingsState createState() => _SettingsState();
}

class _SettingsState extends State<Settings> {
  late TextEditingController _ipController;

  @override
  void initState() {
    super.initState();
    _ipController = TextEditingController(text: widget.ipaddress);
    _ipController.addListener(_updateIp);
  }

  void _updateIp() {
    widget.onIpChanged(_ipController.text);
  }

  @override
  void dispose() {
    _ipController.removeListener(_updateIp);
    _ipController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Text(
            "Project Settings",
            style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
          ),
          const SizedBox(height: 20),

          // IP Address Input Row
          Row(
            children: [
              const Text(
                "IP : ",
                style: TextStyle(fontWeight: FontWeight.bold, fontSize: 20),
              ),
              const SizedBox(width: 10),
              Expanded(
                child: TextField(
                  controller: _ipController,
                  decoration: const InputDecoration(
                    border: OutlineInputBorder(),
                    prefixIcon: Icon(Icons.router),
                    contentPadding: EdgeInsets.symmetric(vertical: 10),
                  ),
                  keyboardType: TextInputType.text,
                ),
              ),
            ],
          ),

          const SizedBox(height: 20),

          // Save Button
          Center(
            child: ElevatedButton(
              onPressed: () {
                ScaffoldMessenger.of(context).showSnackBar(
                  SnackBar(content: Text("IP Address Saved: ${_ipController.text}")),
                );
              },
              child: const Text("Save"),
            ),
          ),
        ],
      ),
    );
  }
}
