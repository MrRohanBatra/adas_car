child: Column(
  children: [
    ElevatedButton(
      onPressed: isOn ? () async => await sendCommand("F") : null,
      child: Icon(Icons.arrow_upward, size: 50),
    ),
    Row(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        ElevatedButton(
          onPressed: isOn ? () async => await sendCommand("L") : null,
          child: Icon(Icons.arrow_back, size: 50),
        ),
        const SizedBox(width: 20),
        ElevatedButton(
          onPressed: isOn ? () async => await sendCommand("R") : null,
          child: Icon(Icons.arrow_forward, size: 50),
        ),
      ],
    ),
    ElevatedButton(
      onPressed: isOn ? () async => await sendCommand("Rev") : null,
      child: Icon(Icons.arrow_downward, size: 50),
    ),
    const SizedBox(height: 20), // Space before special buttons
    Row(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        ElevatedButton(
          onPressed: isOn ? () async => await sendCommand("X") : null,
          child: Icon(Icons.star, size: 50),
        ),
        const SizedBox(width: 20),
        ElevatedButton(
          onPressed: isOn ? () async => await sendCommand("Y") : null,
          child: Icon(Icons.circle, size: 50),
        ),
      ],
    ),
  ],
),
