// File: /menu.js
document.addEventListener("DOMContentLoaded", () => {
    const placeholder = document.getElementById('nav-placeholder');
    
    if (placeholder) {
        fetch('/menu.html')
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok: ' + response.statusText);
                }
                return response.text();
            })
            .then(data => {
                placeholder.innerHTML = data;
            })
            .catch(error => {
                console.error('Error loading the menu:', error);
                placeholder.innerHTML = '<p><em>Navigation failed to load. <a href="/">Return Home</a></em></p>';
            });
    }
});