let todos = JSON.parse(localStorage.getItem('todos')) || [];
let currentFilter = 'all';

function saveTodos() {
    localStorage.setItem('todos', JSON.stringify(todos));
}

function generateId() {
    return Date.now().toString(36) + Math.random().toString(36).substr(2);
}

function createTodoElement(todo) {
    const li = document.createElement('li');
    li.className = `flex items-center gap-3 p-3 border border-hypr-border rounded-lg hover:shadow-lg hover:shadow-hypr-accent/20 transition-all duration-200 ${todo.completed ? 'opacity-50' : ''}`;
    li.dataset.id = todo.id;
    
    li.innerHTML = `
        <input 
            type="checkbox" 
            class="todo-checkbox w-5 h-5 text-hypr-accent rounded focus:ring-2 focus:ring-hypr-accent accent-hypr-accent"
            ${todo.completed ? 'checked' : ''}
            onchange="toggleTodo('${todo.id}')"
        >
        <span class="flex-1 ${todo.completed ? 'line-through text-gray-400' : 'text-hypr-fg'}">
            ${todo.text}
        </span>
        <button 
            onclick="deleteTodo('${todo.id}')"
            class="text-hypr-red hover:text-hypr-red/80 transition-colors duration-200"
        >
            <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16"></path>
            </svg>
        </button>
    `;
    
    return li;
}

function renderTodos() {
    const todoList = document.getElementById('todo-list');
    const emptyState = document.getElementById('empty-state');
    
    let filteredTodos = todos;
    if (currentFilter === 'active') {
        filteredTodos = todos.filter(todo => !todo.completed);
    } else if (currentFilter === 'completed') {
        filteredTodos = todos.filter(todo => todo.completed);
    }
    
    todoList.innerHTML = '';
    
    if (filteredTodos.length === 0) {
        emptyState.classList.remove('hidden');
    } else {
        emptyState.classList.add('hidden');
        filteredTodos.forEach(todo => {
            todoList.appendChild(createTodoElement(todo));
        });
    }
    
    updateFilterButtons();
}

function updateFilterButtons() {
    document.querySelectorAll('.filter-btn').forEach(btn => {
        if (btn.dataset.filter === currentFilter) {
            btn.className = 'filter-btn px-3 py-1 text-sm rounded-md transition-colors duration-200 bg-hypr-accent text-hypr-bg font-medium';
        } else {
            btn.className = 'filter-btn px-3 py-1 text-sm rounded-md transition-colors duration-200 bg-hypr-bg text-hypr-fg hover:bg-hypr-accent/20 border border-hypr-border';
        }
    });
}

function addTodo(text) {
    const todo = {
        id: generateId(),
        text: text,
        completed: false,
        createdAt: new Date().toISOString()
    };
    
    todos.unshift(todo);
    saveTodos();
    renderTodos();
}

function toggleTodo(id) {
    const todo = todos.find(t => t.id === id);
    if (todo) {
        todo.completed = !todo.completed;
        saveTodos();
        renderTodos();
    }
}

function deleteTodo(id) {
    todos = todos.filter(t => t.id !== id);
    saveTodos();
    renderTodos();
}

function filterTodos(filter) {
    currentFilter = filter;
    renderTodos();
}

function clearCompleted() {
    if (confirm('完了済みのタスクをすべて削除しますか？')) {
        todos = todos.filter(todo => !todo.completed);
        saveTodos();
        renderTodos();
    }
}

document.getElementById('todo-form').addEventListener('submit', (e) => {
    e.preventDefault();
    const input = document.getElementById('todo-input');
    const text = input.value.trim();
    
    if (text) {
        addTodo(text);
        input.value = '';
        input.focus();
    }
});

renderTodos();