# Project Structure

## Root Directory Organization

```
project-timeline-app/
├── package.json           # Dependencies and npm scripts
├── server.js             # Express server (main entry point)
├── index.html            # Main application UI
├── data-manager.js       # Data persistence module
├── app.js               # React Ink wireframe tool
├── README.md            # Project documentation
├── TODO.txt             # UI layout reference
├── data/                # JSON data persistence
│   ├── config.json      # Application configuration
│   ├── markdown.json    # Markdown content storage
│   └── posts.json       # Timeline posts storage
├── .kiro/               # Kiro IDE configuration
│   ├── specs/           # Project specifications
│   └── steering/        # AI assistant guidance
└── node_modules/        # Dependencies
```

## Key File Responsibilities

### Core Application Files
- **server.js**: Express server, static file serving, API endpoints
- **index.html**: Complete UI with embedded CSS/JS, dual-pane layout
- **data-manager.js**: localStorage management, JSON persistence, data validation

### Development Tools
- **app.js**: React Ink terminal-based wireframe creation tool
- **test-ui.cjs**: UI testing utilities

### Data Layer
- **data/**: JSON files for persistent storage backup
- **localStorage**: Primary client-side data storage

## UI Component Structure (within index.html)

### Left Pane (320px width)
```
.left-panes
├── .projects-pane (#outline-pane)
├── .categories-pane (#todo-pane)  
└── .filters-pane (#pinned-pane)
```

### Right Pane (flexible width)
```
.timeline-container
├── .timeline-header
├── .post-composer
└── .timeline-item (multiple)
```

### Full-screen Editor Overlay
```
.fullscreen-editor
├── .editor-header
└── .editor-content
    ├── .fullscreen-textarea
    └── .fullscreen-preview
```

## Naming Conventions

### CSS Classes
- **Pane containers**: `[content-type]-pane` (e.g., `projects-pane`)
- **Content areas**: `[content-type]-content` (e.g., `outline-content`)
- **UI components**: `[component]-[element]` (e.g., `post-button`)

### JavaScript Variables
- **DOM elements**: camelCase with descriptive names
- **Data objects**: camelCase with type suffix (e.g., `markdownContent`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `STORAGE_KEYS`)

### File Organization
- **Specs**: `.kiro/specs/project-timeline-app/`
- **Data**: `data/` directory for JSON persistence
- **Assets**: Root level for direct Express static serving