from playwright.sync_api import sync_playwright
import os

def capture_mermaid_diagram():
    with sync_playwright() as p:
        browser = p.chromium.launch()
        page = browser.new_page()
        
        # Navigate to the HTML file
        file_path = os.path.abspath('diagram_temp.html')
        page.goto(f'file://{file_path}')
        
        # Wait for Mermaid to render
        page.wait_for_timeout(2000)
        
        # Take screenshot
        page.screenshot(path='diagram.png', full_page=True)
        
        browser.close()
        print("Diagram saved as diagram.png")

if __name__ == "__main__":
    capture_mermaid_diagram()