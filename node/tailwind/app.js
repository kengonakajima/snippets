let todos = JSON.parse(localStorage.getItem('todos')) || [];
let currentFilter = 'all';
let selectedImage = null;

function saveTodos() {
    localStorage.setItem('todos', JSON.stringify(todos));
}

function generateId() {
    return Date.now().toString(36) + Math.random().toString(36).substr(2);
}

function createTodoElement(todo) {
    const li = document.createElement('li');
    li.className = `${todo.image ? 'flex-col' : 'flex items-center'} gap-2 p-1.5 border border-hypr-border rounded hover:bg-hypr-accent/10 transition-all duration-200 text-xs ${todo.completed ? 'opacity-50' : ''}`;
    li.dataset.id = todo.id;
    
    li.innerHTML = `
        <div class="flex items-center gap-2">
            <input 
                type="checkbox" 
                class="todo-checkbox w-3 h-3 text-hypr-accent rounded focus:ring-1 focus:ring-hypr-accent accent-hypr-accent"
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
                <svg class="w-3 h-3" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16"></path>
                </svg>
            </button>
        </div>
        ${todo.image ? `
            <div class="mt-2 ml-5">
                <img src="${todo.image}" class="max-h-32 rounded border border-hypr-border" alt="Attached image">
            </div>
        ` : ''}
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
            btn.className = 'filter-btn px-2 py-0.5 text-xs rounded transition-colors duration-200 bg-hypr-accent text-hypr-bg font-medium';
        } else {
            btn.className = 'filter-btn px-2 py-0.5 text-xs rounded transition-colors duration-200 bg-hypr-bg text-hypr-fg hover:bg-hypr-accent/20 border border-hypr-border';
        }
    });
}

function addTodo(text, image = null) {
    const todo = {
        id: generateId(),
        text: text,
        image: image,
        completed: false,
        createdAt: new Date().toISOString()
    };
    
    todos.unshift(todo);
    
    try {
        saveTodos();
        renderTodos();
    } catch (e) {
        if (e.name === 'QuotaExceededError') {
            // 保存に失敗したらtodoを削除
            todos.shift();
            alert('ストレージ容量が不足しています。画像サイズを小さくするか、既存のデータを削除してください。');
            
            // エラー時の対処法を表示
            console.error('LocalStorage quota exceeded. Current usage:', checkStorageUsage());
        } else {
            throw e;
        }
    }
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
    if (confirm('Clear all completed tasks?')) {
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
        addTodo(text, selectedImage);
        input.value = '';
        clearImage();
        input.focus();
    }
});

function handleImageSelect(event) {
    const file = event.target.files[0];
    if (file && file.type.startsWith('image/')) {
        const reader = new FileReader();
        reader.onload = (e) => {
            const img = new Image();
            img.onload = function() {
                // 画像をリサイズ
                const maxWidth = 400;
                const maxHeight = 400;
                let width = img.width;
                let height = img.height;
                
                if (width > height) {
                    if (width > maxWidth) {
                        height = height * (maxWidth / width);
                        width = maxWidth;
                    }
                } else {
                    if (height > maxHeight) {
                        width = width * (maxHeight / height);
                        height = maxHeight;
                    }
                }
                
                // Canvas を使ってリサイズ
                const canvas = document.createElement('canvas');
                canvas.width = width;
                canvas.height = height;
                
                const ctx = canvas.getContext('2d');
                ctx.drawImage(img, 0, 0, width, height);
                
                // JPEGとして圧縮（品質50%でさらに軽量化）
                selectedImage = canvas.toDataURL('image/jpeg', 0.5);
                document.getElementById('preview-img').src = selectedImage;
                document.getElementById('image-preview').classList.remove('hidden');
            };
            img.src = e.target.result;
        };
        reader.readAsDataURL(file);
    }
}

function clearImage() {
    selectedImage = null;
    document.getElementById('image-input').value = '';
    document.getElementById('image-preview').classList.add('hidden');
}

function clearAllData() {
    if (confirm('すべてのデータを削除しますか？この操作は取り消せません。')) {
        localStorage.removeItem('todos');
        todos = [];
        renderTodos();
    }
}

// LocalStorageの使用量を確認
function checkStorageUsage() {
    let totalSize = 0;
    for (let key in localStorage) {
        if (localStorage.hasOwnProperty(key)) {
            totalSize += localStorage[key].length + key.length;
        }
    }
    // 概算のMB表示
    const sizeMB = (totalSize / 1024 / 1024).toFixed(2);
    console.log(`LocalStorage usage: ${sizeMB} MB`);
    
    // 使用率が80%を超えたら警告
    if (totalSize > 4 * 1024 * 1024) {
        console.warn('LocalStorage usage is high. Consider clearing old data.');
    }
    
    return sizeMB;
}

renderTodos();
checkStorageUsage();