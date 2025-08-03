# Technology Stack

## Frontend
- **HTML5/CSS3**: Modern semantic markup and responsive design
- **Vanilla JavaScript**: No framework dependencies for simplicity
- **marked.js**: Markdown parsing and rendering (loaded via CDN)
- **localStorage**: Client-side data persistence

## Backend
- **Node.js**: Runtime environment
- **Express.js**: Web server framework
- **ES Modules**: Modern JavaScript module system (`"type": "module"`)

## Key Dependencies
```json
{
  "express": "^4.18.0",
  "open": "^8.4.0", 
  "cors": "^2.8.5",
  "react": "^19.1.0",
  "ink": "^6.0.1"
}
```

## Development Tools
- **Babel**: For React/JSX transpilation (wireframe tool)
- **Playwright**: Browser automation testing
- **npm scripts**: Build and development workflow

## Common Commands

### Development
```bash
npm start          # Start local development server
npm test           # Run tests (placeholder)
```

### Server Management
- Server runs on port 3000 by default
- Auto-opens browser on startup
- Graceful shutdown with Ctrl+C
- CORS enabled for development

## Architecture Patterns
- **Module-based**: Separate concerns (data-manager.js, server.js)
- **Event-driven**: DOM event handling for UI interactions
- **Offline-first**: localStorage with optional JSON file persistence
- **Static file serving**: Express serves all frontend assets
- **RESTful API**: `/api/save-data` and `/api/load-data` endpoints

## File Structure
```
├── server.js              # Express server
├── index.html            # Main application
├── data-manager.js       # Data persistence layer
├── app.js               # React Ink wireframe tool
├── data/                # JSON data files
└── .kiro/specs/         # Project specifications
```