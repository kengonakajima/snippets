# Simple Voxel Game

A simple voxel-based game built with Three.js.

## Features

- 3D voxel-based environment
- Click to add blocks (left-click)
- Right-click to remove blocks
- Orbit controls to navigate the scene

## How to Run

You'll need to serve the files through a local web server due to JavaScript module security restrictions.

Using Python:
```
python -m http.server
```

Or with Node.js and a package like `http-server`:
```
npx http-server
```

Then navigate to `http://localhost:8000` in your web browser.

## Controls

- Click on screen to lock mouse and start playing
- WASD: Move around
- Space: Jump
- Mouse: Look around
- Left-click: Add a voxel
- Right-click: Remove a voxel
- Press '1': Select brown block
- Press '2': Select grass block (green top with dirt sides)
- ESC: Exit pointer lock mode
