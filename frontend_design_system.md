# Frontend Design System: Screen-to-RTSP ONVIF Bridge

## Table of Contents
1. [Visual Design System](#1-visual-design-system)
2. [System Tray Design](#2-system-tray-design)
3. [Web Configuration Interface Design](#3-web-configuration-interface-design)
4. [Micro-interactions](#4-micro-interactions)
5. [Accessibility Implementation](#5-accessibility-implementation)
6. [Performance Considerations](#6-performance-considerations)
7. [Implementation Guidelines](#7-implementation-guidelines)

## 1. Visual Design System

### 1.1 Color Palette

#### Light Theme
```css
:root {
  /* Primary Colors */
  --primary-500: #2563eb; /* Main brand color */
  --primary-600: #1d4ed8; /* Hover state */
  --primary-700: #1e40af; /* Active state */
  --primary-100: #dbeafe; /* Light backgrounds */
  
  /* Neutral Colors */
  --gray-50: #f9fafb;
  --gray-100: #f3f4f6;
  --gray-200: #e5e7eb;
  --gray-300: #d1d5db;
  --gray-400: #9ca3af;
  --gray-500: #6b7280;
  --gray-600: #4b5563;
  --gray-700: #374151;
  --gray-800: #1f2937;
  --gray-900: #111827;
  
  /* Semantic Colors */
  --success-500: #10b981; /* Streaming active */
  --warning-500: #f59e0b; /* Configuration needed */
  --error-500: #ef4444;   /* Errors/stopped */
  --info-500: #3b82f6;    /* Information */
  
  /* Background & Text */
  --bg-primary: #ffffff;
  --bg-secondary: var(--gray-50);
  --bg-tertiary: var(--gray-100);
  --text-primary: var(--gray-900);
  --text-secondary: var(--gray-600);
  --text-disabled: var(--gray-400);
  --border-color: var(--gray-200);
}
```

#### Dark Theme
```css
[data-theme="dark"] {
  /* Inverted neutrals */
  --bg-primary: #0f172a;
  --bg-secondary: #1e293b;
  --bg-tertiary: #334155;
  --text-primary: #f1f5f9;
  --text-secondary: #cbd5e1;
  --text-disabled: #64748b;
  --border-color: #334155;
  
  /* Adjusted semantic colors */
  --primary-500: #3b82f6;
  --primary-600: #2563eb;
  --success-500: #22c55e;
  --warning-500: #facc15;
  --error-500: #f87171;
}
```

### 1.2 Typography Scale

```css
/* System Font Stack */
--font-family-sans: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, 
                    "Helvetica Neue", Arial, sans-serif;
--font-family-mono: "SF Mono", Monaco, "Cascadia Code", "Roboto Mono", 
                    Consolas, "Courier New", monospace;

/* Type Scale */
--text-xs: 0.75rem;    /* 12px */
--text-sm: 0.875rem;   /* 14px */
--text-base: 1rem;     /* 16px */
--text-lg: 1.125rem;   /* 18px */
--text-xl: 1.25rem;    /* 20px */
--text-2xl: 1.5rem;    /* 24px */
--text-3xl: 1.875rem;  /* 30px */

/* Line Heights */
--leading-tight: 1.25;
--leading-normal: 1.5;
--leading-relaxed: 1.75;

/* Font Weights */
--font-normal: 400;
--font-medium: 500;
--font-semibold: 600;
--font-bold: 700;
```

### 1.3 Spacing System

```css
/* 4px base unit */
--space-0: 0;
--space-1: 0.25rem;  /* 4px */
--space-2: 0.5rem;   /* 8px */
--space-3: 0.75rem;  /* 12px */
--space-4: 1rem;     /* 16px */
--space-5: 1.25rem;  /* 20px */
--space-6: 1.5rem;   /* 24px */
--space-8: 2rem;     /* 32px */
--space-10: 2.5rem;  /* 40px */
--space-12: 3rem;    /* 48px */
--space-16: 4rem;    /* 64px */
```

### 1.4 Component Library

#### Buttons
```css
/* Base Button */
.btn {
  font-family: var(--font-family-sans);
  font-weight: var(--font-medium);
  font-size: var(--text-sm);
  line-height: var(--leading-normal);
  padding: var(--space-2) var(--space-4);
  border-radius: 0.375rem;
  transition: all 150ms ease-in-out;
  cursor: pointer;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  gap: var(--space-2);
  border: 1px solid transparent;
  min-height: 2.5rem; /* 40px - touch target */
}

/* Primary Button */
.btn-primary {
  background-color: var(--primary-500);
  color: white;
  border-color: var(--primary-500);
}

.btn-primary:hover {
  background-color: var(--primary-600);
  border-color: var(--primary-600);
}

.btn-primary:active {
  background-color: var(--primary-700);
  border-color: var(--primary-700);
}

/* Secondary Button */
.btn-secondary {
  background-color: transparent;
  color: var(--text-primary);
  border-color: var(--border-color);
}

.btn-secondary:hover {
  background-color: var(--bg-secondary);
  border-color: var(--gray-300);
}

/* Danger Button */
.btn-danger {
  background-color: var(--error-500);
  color: white;
  border-color: var(--error-500);
}

/* Button Sizes */
.btn-sm {
  font-size: var(--text-xs);
  padding: var(--space-1) var(--space-3);
  min-height: 2rem; /* 32px */
}

.btn-lg {
  font-size: var(--text-base);
  padding: var(--space-3) var(--space-6);
  min-height: 3rem; /* 48px */
}

/* Button States */
.btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.btn-loading {
  color: transparent;
  position: relative;
}

.btn-loading::after {
  content: "";
  position: absolute;
  width: 1rem;
  height: 1rem;
  border: 2px solid currentColor;
  border-right-color: transparent;
  border-radius: 50%;
  animation: spin 0.75s linear infinite;
}
```

#### Form Controls
```css
/* Input Base */
.input {
  width: 100%;
  padding: var(--space-2) var(--space-3);
  font-size: var(--text-sm);
  line-height: var(--leading-normal);
  color: var(--text-primary);
  background-color: var(--bg-primary);
  border: 1px solid var(--border-color);
  border-radius: 0.375rem;
  transition: border-color 150ms ease-in-out;
  min-height: 2.5rem; /* 40px - touch target */
}

.input:hover {
  border-color: var(--gray-300);
}

.input:focus {
  outline: none;
  border-color: var(--primary-500);
  box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.1);
}

/* Select */
.select {
  appearance: none;
  background-image: url("data:image/svg+xml;charset=UTF-8,%3csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='none' stroke='currentColor' stroke-width='2'%3e%3cpath d='M6 9l6 6 6-6'/%3e%3c/svg%3e");
  background-repeat: no-repeat;
  background-position: right var(--space-2) center;
  background-size: 1.5rem;
  padding-right: var(--space-10);
}

/* Checkbox & Radio */
.checkbox,
.radio {
  width: 1.25rem;
  height: 1.25rem;
  color: var(--primary-500);
  border: 1px solid var(--border-color);
  border-radius: 0.25rem;
  cursor: pointer;
}

.radio {
  border-radius: 50%;
}

/* Toggle Switch */
.toggle {
  position: relative;
  width: 3rem;
  height: 1.5rem;
  background-color: var(--gray-300);
  border-radius: 9999px;
  transition: background-color 200ms;
  cursor: pointer;
}

.toggle-input:checked + .toggle {
  background-color: var(--primary-500);
}

.toggle-slider {
  position: absolute;
  top: 0.125rem;
  left: 0.125rem;
  width: 1.25rem;
  height: 1.25rem;
  background-color: white;
  border-radius: 50%;
  transition: transform 200ms;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
}

.toggle-input:checked + .toggle .toggle-slider {
  transform: translateX(1.5rem);
}
```

#### Cards & Containers
```css
/* Card Component */
.card {
  background-color: var(--bg-primary);
  border: 1px solid var(--border-color);
  border-radius: 0.5rem;
  padding: var(--space-4);
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.1);
}

.card-header {
  font-size: var(--text-lg);
  font-weight: var(--font-semibold);
  margin-bottom: var(--space-3);
  padding-bottom: var(--space-3);
  border-bottom: 1px solid var(--border-color);
}

.card-body {
  /* Content area */
}

.card-footer {
  margin-top: var(--space-4);
  padding-top: var(--space-3);
  border-top: 1px solid var(--border-color);
}

/* Section Container */
.section {
  padding: var(--space-6);
  margin-bottom: var(--space-4);
}

.section-title {
  font-size: var(--text-2xl);
  font-weight: var(--font-bold);
  margin-bottom: var(--space-2);
}

.section-subtitle {
  font-size: var(--text-base);
  color: var(--text-secondary);
  margin-bottom: var(--space-4);
}
```

#### Status Indicators
```css
/* Status Badge */
.badge {
  display: inline-flex;
  align-items: center;
  padding: var(--space-1) var(--space-2);
  font-size: var(--text-xs);
  font-weight: var(--font-medium);
  border-radius: 9999px;
  text-transform: uppercase;
  letter-spacing: 0.025em;
}

.badge-success {
  background-color: rgba(16, 185, 129, 0.1);
  color: var(--success-500);
}

.badge-warning {
  background-color: rgba(245, 158, 11, 0.1);
  color: var(--warning-500);
}

.badge-error {
  background-color: rgba(239, 68, 68, 0.1);
  color: var(--error-500);
}

/* Progress Bar */
.progress {
  width: 100%;
  height: 0.5rem;
  background-color: var(--gray-200);
  border-radius: 9999px;
  overflow: hidden;
}

.progress-bar {
  height: 100%;
  background-color: var(--primary-500);
  transition: width 300ms ease-in-out;
}

/* Loading Spinner */
.spinner {
  width: 2rem;
  height: 2rem;
  border: 3px solid var(--gray-200);
  border-top-color: var(--primary-500);
  border-radius: 50%;
  animation: spin 1s linear infinite;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}
```

### 1.5 Icons System

```css
/* Icon Base */
.icon {
  width: 1.25rem;
  height: 1.25rem;
  display: inline-block;
  flex-shrink: 0;
}

.icon-sm { 
  width: 1rem; 
  height: 1rem; 
}

.icon-lg { 
  width: 1.5rem; 
  height: 1.5rem; 
}

/* Common Icons (SVG sprites) */
.icon-camera { /* Streaming */ }
.icon-settings { /* Configuration */ }
.icon-play { /* Start */ }
.icon-stop { /* Stop */ }
.icon-refresh { /* Restart */ }
.icon-check { /* Success */ }
.icon-x { /* Error/Close */ }
.icon-alert { /* Warning */ }
.icon-info { /* Information */ }
.icon-monitor { /* Screen/Display */ }
```

## 2. System Tray Design

### 2.1 Icon States and Animations

#### State Icons (16x16, 32x32, 64x64)
```yaml
Inactive:
  icon: camera_outline_gray
  animation: none
  tooltip: "Screen to RTSP - Click to configure"

Ready:
  icon: camera_outline_blue
  animation: subtle_pulse (2s)
  tooltip: "Ready to stream - Click to start"

Streaming:
  icon: camera_filled_green
  animation: recording_dot (1s blink)
  tooltip: "Streaming to {count} clients"

Error:
  icon: camera_error_red
  animation: attention_shake (0.5s)
  tooltip: "Error: {error_message}"

Configuring:
  icon: camera_gear_yellow
  animation: rotate_gear (2s)
  tooltip: "Configuration in progress..."
```

### 2.2 Menu Structure

```
┌─────────────────────────────────┐
│ ● Screen to RTSP               │ (Header with status)
├─────────────────────────────────┤
│ ▶ Start Streaming              │ (Primary action)
│ ⚙ Open Configuration           │
│ 📊 View Status                 │
├─────────────────────────────────┤
│ 🖥 Screen: Primary Monitor     │ (Current selection)
│ 📹 Profile: Default (1080p30)  │
├─────────────────────────────────┤
│ 📋 Copy Stream URL             │
│ 🔍 Show Logs                   │
├─────────────────────────────────┤
│ ❌ Exit                        │
└─────────────────────────────────┘
```

### 2.3 Platform-Specific Considerations

#### Windows
```cpp
// Use Shell_NotifyIcon API
// Support Windows 10/11 notification center
// Dark mode detection via registry
// High DPI awareness required
```

#### macOS
```objc
// Use NSStatusItem
// Support macOS menu bar styling
// Implement NSMenu with custom views
// Support Retina displays
```

#### Linux
```cpp
// Use libappindicator or StatusNotifierItem
// Support both Unity and GNOME
// Fallback to system tray if needed
// Theme detection via GTK
```

## 3. Web Configuration Interface Design

### 3.1 Page Layout Structure

```html
<!-- Main Layout Grid -->
<div class="app-container">
  <header class="app-header">
    <h1 class="app-title">Screen to RTSP Configuration</h1>
    <div class="header-actions">
      <button class="btn-icon theme-toggle">🌙</button>
      <button class="btn-icon help">❓</button>
    </div>
  </header>
  
  <nav class="app-nav">
    <a class="nav-item active" href="#dashboard">Dashboard</a>
    <a class="nav-item" href="#video">Video Settings</a>
    <a class="nav-item" href="#network">Network</a>
    <a class="nav-item" href="#advanced">Advanced</a>
  </nav>
  
  <main class="app-content">
    <!-- Dynamic content area -->
  </main>
  
  <footer class="app-footer">
    <span class="status-indicator">● Connected</span>
    <span class="version">v1.0.0</span>
  </footer>
</div>
```

### 3.2 Dashboard Design

```html
<!-- Status Card -->
<div class="dashboard-grid">
  <div class="status-card streaming">
    <div class="status-icon">📹</div>
    <div class="status-content">
      <h3>Streaming Active</h3>
      <p>rtsp://192.168.1.100:554/stream</p>
      <div class="status-metrics">
        <span>3 clients</span>
        <span>5.2 Mbps</span>
        <span>30 fps</span>
      </div>
    </div>
    <button class="btn btn-danger">Stop Stream</button>
  </div>
  
  <!-- Quick Stats -->
  <div class="stats-grid">
    <div class="stat-card">
      <div class="stat-value">15%</div>
      <div class="stat-label">CPU Usage</div>
      <div class="stat-chart"><!-- Mini chart --></div>
    </div>
    <!-- More stat cards... -->
  </div>
</div>
```

### 3.3 Settings Forms

```html
<!-- Video Settings Section -->
<section class="settings-section">
  <h2 class="section-title">Video Settings</h2>
  
  <div class="form-group">
    <label for="codec" class="form-label">
      Codec
      <span class="tooltip" data-tip="H.264 is more compatible, H.265 is more efficient">ℹ</span>
    </label>
    <select id="codec" class="select">
      <option value="h264">H.264 (Most Compatible)</option>
      <option value="h265">H.265 (Better Compression)</option>
    </select>
  </div>
  
  <div class="form-group">
    <label for="resolution" class="form-label">Resolution</label>
    <div class="radio-group">
      <label class="radio-label">
        <input type="radio" name="resolution" value="1920x1080" class="radio">
        <span>1080p (1920×1080)</span>
      </label>
      <!-- More options... -->
    </div>
  </div>
  
  <div class="form-group">
    <label for="fps" class="form-label">Frame Rate</label>
    <div class="slider-container">
      <input type="range" id="fps" min="15" max="60" value="30" class="slider">
      <output class="slider-value">30 fps</output>
    </div>
  </div>
</section>
```

### 3.4 Responsive Design

```css
/* Mobile First Approach */
.app-container {
  display: grid;
  grid-template-rows: auto auto 1fr auto;
  min-height: 100vh;
}

/* Tablet (768px+) */
@media (min-width: 768px) {
  .app-container {
    grid-template-columns: 200px 1fr;
    grid-template-rows: auto 1fr auto;
  }
  
  .app-nav {
    grid-column: 1;
    grid-row: 2;
  }
  
  .app-content {
    grid-column: 2;
    grid-row: 2;
  }
}

/* Desktop (1024px+) */
@media (min-width: 1024px) {
  .app-container {
    grid-template-columns: 250px 1fr;
  }
  
  .dashboard-grid {
    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  }
}
```

## 4. Micro-interactions

### 4.1 Button States

```css
/* Hover */
.btn:hover {
  transform: translateY(-1px);
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

/* Active/Click */
.btn:active {
  transform: translateY(0);
  box-shadow: 0 1px 2px rgba(0, 0, 0, 0.1);
}

/* Focus */
.btn:focus-visible {
  outline: 2px solid var(--primary-500);
  outline-offset: 2px;
}

/* Loading */
.btn.is-loading {
  pointer-events: none;
  position: relative;
  color: transparent;
}

.btn.is-loading::after {
  content: "";
  position: absolute;
  inset: 0;
  margin: auto;
  width: 1rem;
  height: 1rem;
  border: 2px solid currentColor;
  border-right-color: transparent;
  border-radius: 50%;
  animation: spin 0.75s linear infinite;
}
```

### 4.2 Form Validation

```css
/* Valid State */
.input.is-valid {
  border-color: var(--success-500);
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='20' height='20' viewBox='0 0 20 20' fill='%2310b981'%3E%3Cpath fill-rule='evenodd' d='M16.707 5.293a1 1 0 010 1.414l-8 8a1 1 0 01-1.414 0l-4-4a1 1 0 011.414-1.414L8 12.586l7.293-7.293a1 1 0 011.414 0z' clip-rule='evenodd'%3E%3C/path%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: right 0.75rem center;
  padding-right: 2.5rem;
}

/* Invalid State */
.input.is-invalid {
  border-color: var(--error-500);
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='20' height='20' viewBox='0 0 20 20' fill='%23ef4444'%3E%3Cpath fill-rule='evenodd' d='M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7 4a1 1 0 11-2 0 1 1 0 012 0zm-1-9a1 1 0 00-1 1v4a1 1 0 102 0V6a1 1 0 00-1-1z' clip-rule='evenodd'%3E%3C/path%3E%3C/svg%3E");
}

/* Error Message */
.form-error {
  color: var(--error-500);
  font-size: var(--text-sm);
  margin-top: var(--space-1);
  animation: slideDown 200ms ease-out;
}

@keyframes slideDown {
  from {
    opacity: 0;
    transform: translateY(-0.5rem);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}
```

### 4.3 Progress Indicators

```css
/* Determinate Progress */
.progress-determinate {
  position: relative;
  height: 0.5rem;
  background-color: var(--gray-200);
  border-radius: 9999px;
  overflow: hidden;
}

.progress-bar {
  height: 100%;
  background-color: var(--primary-500);
  transition: width 300ms ease-out;
}

/* Indeterminate Progress */
.progress-indeterminate .progress-bar {
  width: 30%;
  animation: indeterminate 1.5s ease-in-out infinite;
}

@keyframes indeterminate {
  0% { transform: translateX(-100%); }
  100% { transform: translateX(400%); }
}

/* Circular Progress */
.progress-circle {
  width: 3rem;
  height: 3rem;
  position: relative;
}

.progress-circle svg {
  transform: rotate(-90deg);
}

.progress-circle-track {
  stroke: var(--gray-200);
  fill: none;
  stroke-width: 4;
}

.progress-circle-fill {
  stroke: var(--primary-500);
  fill: none;
  stroke-width: 4;
  stroke-linecap: round;
  stroke-dasharray: 100;
  stroke-dashoffset: 75; /* 25% progress */
  transition: stroke-dashoffset 300ms ease-out;
}
```

### 4.4 Success/Error Animations

```css
/* Success Checkmark */
.success-animation {
  width: 4rem;
  height: 4rem;
  margin: 0 auto;
}

.success-circle {
  stroke: var(--success-500);
  stroke-width: 2;
  fill: none;
  animation: drawCircle 0.6s ease-out;
}

.success-check {
  stroke: var(--success-500);
  stroke-width: 2;
  fill: none;
  stroke-dasharray: 48;
  stroke-dashoffset: 48;
  animation: drawCheck 0.3s 0.6s ease-out forwards;
}

@keyframes drawCircle {
  from {
    stroke-dasharray: 0 151;
  }
  to {
    stroke-dasharray: 151 151;
  }
}

@keyframes drawCheck {
  to {
    stroke-dashoffset: 0;
  }
}

/* Error Animation */
.error-animation {
  animation: shake 0.5s ease-in-out;
}

@keyframes shake {
  0%, 100% { transform: translateX(0); }
  10%, 30%, 50%, 70%, 90% { transform: translateX(-2px); }
  20%, 40%, 60%, 80% { transform: translateX(2px); }
}
```

## 5. Accessibility Implementation

### 5.1 ARIA Patterns

```html
<!-- Live Region for Status Updates -->
<div role="status" aria-live="polite" aria-atomic="true" class="sr-only">
  <span id="status-message">Streaming started successfully</span>
</div>

<!-- Form with ARIA -->
<form role="form" aria-labelledby="form-title">
  <h2 id="form-title">Video Configuration</h2>
  
  <div class="form-group">
    <label for="bitrate" id="bitrate-label">
      Bitrate
      <span class="required" aria-label="required">*</span>
    </label>
    <input 
      type="number" 
      id="bitrate" 
      aria-labelledby="bitrate-label"
      aria-describedby="bitrate-help bitrate-error"
      aria-invalid="false"
      aria-required="true"
      class="input"
    >
    <span id="bitrate-help" class="form-help">
      Enter bitrate in Kbps (1000-50000)
    </span>
    <span id="bitrate-error" class="form-error" role="alert" aria-live="assertive">
      <!-- Error message inserted here -->
    </span>
  </div>
</form>

<!-- Tab Navigation -->
<div role="tablist" aria-label="Settings sections">
  <button 
    role="tab" 
    aria-selected="true" 
    aria-controls="panel-video"
    id="tab-video"
    tabindex="0"
  >
    Video Settings
  </button>
  <button 
    role="tab" 
    aria-selected="false" 
    aria-controls="panel-network"
    id="tab-network"
    tabindex="-1"
  >
    Network Settings
  </button>
</div>

<div 
  role="tabpanel" 
  id="panel-video" 
  aria-labelledby="tab-video"
  tabindex="0"
>
  <!-- Video settings content -->
</div>
```

### 5.2 Keyboard Navigation

```javascript
// Focus Management
class FocusManager {
  constructor(container) {
    this.container = container;
    this.focusableElements = this.getFocusableElements();
    this.currentIndex = 0;
  }
  
  getFocusableElements() {
    const selector = 'a, button, input, select, textarea, [tabindex]:not([tabindex="-1"])';
    return Array.from(this.container.querySelectorAll(selector))
      .filter(el => !el.disabled && el.offsetParent !== null);
  }
  
  focusFirst() {
    if (this.focusableElements.length > 0) {
      this.focusableElements[0].focus();
      this.currentIndex = 0;
    }
  }
  
  focusNext() {
    this.currentIndex = (this.currentIndex + 1) % this.focusableElements.length;
    this.focusableElements[this.currentIndex].focus();
  }
  
  focusPrevious() {
    this.currentIndex = (this.currentIndex - 1 + this.focusableElements.length) % this.focusableElements.length;
    this.focusableElements[this.currentIndex].focus();
  }
}

// Keyboard Shortcuts
document.addEventListener('keydown', (e) => {
  // Global shortcuts
  if (e.ctrlKey || e.metaKey) {
    switch (e.key) {
      case 's': // Save
        e.preventDefault();
        saveConfiguration();
        break;
      case '/': // Focus search
        e.preventDefault();
        document.getElementById('search')?.focus();
        break;
    }
  }
  
  // Tab navigation
  if (e.key === 'Tab') {
    const activeElement = document.activeElement;
    if (activeElement.getAttribute('role') === 'tab') {
      e.preventDefault();
      navigateTabs(e.shiftKey ? 'previous' : 'next');
    }
  }
  
  // Escape key
  if (e.key === 'Escape') {
    closeModal();
    cancelOperation();
  }
});

// Skip Links
```html
<a href="#main-content" class="skip-link">Skip to main content</a>
<a href="#navigation" class="skip-link">Skip to navigation</a>
```

### 5.3 Focus Management

```css
/* Focus Indicators */
:focus {
  outline: 2px solid var(--primary-500);
  outline-offset: 2px;
}

/* Remove default outline but keep for keyboard navigation */
:focus:not(:focus-visible) {
  outline: none;
}

:focus-visible {
  outline: 2px solid var(--primary-500);
  outline-offset: 2px;
}

/* Focus trap for modals */
.modal-focus-trap {
  position: fixed;
  inset: 0;
  z-index: 9999;
}

/* Skip links */
.skip-link {
  position: absolute;
  top: -40px;
  left: 0;
  background: var(--primary-500);
  color: white;
  padding: var(--space-2) var(--space-4);
  text-decoration: none;
  border-radius: 0 0 0.25rem 0;
  z-index: 100;
}

.skip-link:focus {
  top: 0;
}
```

### 5.4 Screen Reader Optimization

```html
<!-- Descriptive Labels -->
<button 
  class="btn btn-icon" 
  aria-label="Start streaming with current configuration"
  title="Start streaming"
>
  <svg class="icon icon-play" aria-hidden="true">
    <!-- Icon SVG -->
  </svg>
  <span class="sr-only">Start streaming</span>
</button>

<!-- Table with Screen Reader Support -->
<table role="table" aria-label="Active client connections">
  <caption class="sr-only">List of currently connected RTSP clients</caption>
  <thead>
    <tr role="row">
      <th role="columnheader" scope="col">Client IP</th>
      <th role="columnheader" scope="col">Connection Time</th>
      <th role="columnheader" scope="col">Bandwidth</th>
      <th role="columnheader" scope="col">Actions</th>
    </tr>
  </thead>
  <tbody>
    <tr role="row">
      <td role="cell">192.168.1.100</td>
      <td role="cell">5 minutes</td>
      <td role="cell">2.3 Mbps</td>
      <td role="cell">
        <button aria-label="Disconnect client 192.168.1.100">
          Disconnect
        </button>
      </td>
    </tr>
  </tbody>
</table>

<!-- Loading States -->
<div class="loading" role="status" aria-live="polite">
  <span class="spinner" aria-hidden="true"></span>
  <span class="sr-only">Loading configuration...</span>
</div>
```

## 6. Performance Considerations

### 6.1 CSS Optimization

```css
/* Critical CSS (inline in HTML) */
:root {
  /* Core custom properties */
}

body {
  margin: 0;
  font-family: var(--font-family-sans);
  color: var(--text-primary);
  background-color: var(--bg-primary);
}

.app-container {
  display: grid;
  min-height: 100vh;
}

/* Above-the-fold styles */
.app-header,
.status-card {
  /* Critical layout styles */
}

/* Non-critical CSS (external file) */
/* Animations, transitions, decorative styles */
```

### 6.2 Lazy Loading

```javascript
// Image Lazy Loading
const lazyImages = document.querySelectorAll('img[data-src]');
const imageObserver = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      const img = entry.target;
      img.src = img.dataset.src;
      img.removeAttribute('data-src');
      imageObserver.unobserve(img);
    }
  });
});

lazyImages.forEach(img => imageObserver.observe(img));

// Component Lazy Loading
const loadAdvancedSettings = async () => {
  const module = await import('./advanced-settings.js');
  module.init();
};

// Intersection Observer for deferred loading
const deferredSections = document.querySelectorAll('[data-defer]');
const sectionObserver = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      const section = entry.target;
      loadSectionContent(section.dataset.defer);
      sectionObserver.unobserve(section);
    }
  });
}, { rootMargin: '50px' });

deferredSections.forEach(section => sectionObserver.observe(section));
```

### 6.3 Animation Performance

```css
/* Use transform and opacity for animations */
.animate-slide-in {
  animation: slideIn 300ms ease-out;
}

@keyframes slideIn {
  from {
    opacity: 0;
    transform: translateY(1rem);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

/* Hardware acceleration hints */
.will-animate {
  will-change: transform, opacity;
}

/* Reduce motion preference */
@media (prefers-reduced-motion: reduce) {
  *,
  *::before,
  *::after {
    animation-duration: 0.01ms !important;
    animation-iteration-count: 1 !important;
    transition-duration: 0.01ms !important;
  }
}

/* Efficient animations */
.pulse {
  animation: pulse 2s cubic-bezier(0.4, 0, 0.6, 1) infinite;
}

@keyframes pulse {
  0%, 100% {
    opacity: 1;
  }
  50% {
    opacity: 0.5;
  }
}
```

### 6.4 Resource-Light Approaches

```javascript
// Debounced Input Handling
function debounce(func, wait) {
  let timeout;
  return function executedFunction(...args) {
    const later = () => {
      clearTimeout(timeout);
      func(...args);
    };
    clearTimeout(timeout);
    timeout = setTimeout(later, wait);
  };
}

const handleSearch = debounce((value) => {
  searchSettings(value);
}, 300);

// Throttled Scroll Events
function throttle(func, limit) {
  let inThrottle;
  return function(...args) {
    if (!inThrottle) {
      func.apply(this, args);
      inThrottle = true;
      setTimeout(() => inThrottle = false, limit);
    }
  };
}

const handleScroll = throttle(() => {
  updateScrollPosition();
}, 100);

// Virtual Scrolling for Long Lists
class VirtualScroller {
  constructor(container, items, itemHeight) {
    this.container = container;
    this.items = items;
    this.itemHeight = itemHeight;
    this.visibleItems = Math.ceil(container.clientHeight / itemHeight);
    this.render();
  }
  
  render() {
    const scrollTop = this.container.scrollTop;
    const startIndex = Math.floor(scrollTop / this.itemHeight);
    const endIndex = Math.min(
      startIndex + this.visibleItems + 1,
      this.items.length
    );
    
    // Render only visible items
    const fragment = document.createDocumentFragment();
    for (let i = startIndex; i < endIndex; i++) {
      const item = this.createItemElement(this.items[i]);
      item.style.position = 'absolute';
      item.style.top = `${i * this.itemHeight}px`;
      fragment.appendChild(item);
    }
    
    this.container.innerHTML = '';
    this.container.appendChild(fragment);
    this.container.style.height = `${this.items.length * this.itemHeight}px`;
  }
}

// Memory-Efficient Event Handling
const eventDelegation = {
  init() {
    // Single event listener for multiple elements
    document.addEventListener('click', this.handleClick.bind(this));
  },
  
  handleClick(e) {
    const button = e.target.closest('.btn');
    if (button) {
      const action = button.dataset.action;
      this.executeAction(action, button);
    }
  },
  
  executeAction(action, element) {
    switch (action) {
      case 'start-stream':
        startStreaming();
        break;
      case 'save-config':
        saveConfiguration();
        break;
      // More actions...
    }
  }
};
```

## 7. Implementation Guidelines

### 7.1 Development Workflow

```bash
# File Structure
frontend/
├── index.html          # Main HTML file
├── css/
│   ├── critical.css    # Inline critical styles
│   ├── main.css        # Main stylesheet
│   ├── components.css  # Component styles
│   └── themes.css      # Theme variations
├── js/
│   ├── app.js          # Main application
│   ├── config.js       # Configuration module
│   ├── api.js          # API communication
│   └── components/     # UI components
└── assets/
    ├── icons/          # SVG icons
    └── images/         # Static images
```

### 7.2 Build Process

```javascript
// Minimal build script (no framework required)
const fs = require('fs');
const path = require('path');

// CSS Processing
function processCSS() {
  // Combine CSS files
  const cssFiles = [
    'css/critical.css',
    'css/main.css',
    'css/components.css',
    'css/themes.css'
  ];
  
  const combined = cssFiles
    .map(file => fs.readFileSync(file, 'utf8'))
    .join('\n');
  
  // Basic minification
  const minified = combined
    .replace(/\/\*[\s\S]*?\*\//g, '') // Remove comments
    .replace(/\s+/g, ' ')             // Collapse whitespace
    .replace(/\s*{\s*/g, '{')         // Remove spaces around braces
    .replace(/\s*}\s*/g, '}')
    .replace(/\s*:\s*/g, ':')
    .replace(/\s*;\s*/g, ';');
  
  fs.writeFileSync('dist/styles.min.css', minified);
}

// JavaScript Processing
function processJS() {
  // Simple concatenation for vanilla JS
  const jsFiles = [
    'js/api.js',
    'js/config.js',
    'js/components/button.js',
    'js/components/form.js',
    'js/app.js'
  ];
  
  const combined = jsFiles
    .map(file => fs.readFileSync(file, 'utf8'))
    .join('\n');
  
  fs.writeFileSync('dist/app.js', combined);
}
```

### 7.3 Browser Compatibility

```css
/* Progressive Enhancement */
/* Base styles work everywhere */
.btn {
  display: inline-block;
  padding: 0.5rem 1rem;
  background: #2563eb;
  color: white;
  text-decoration: none;
}

/* Modern browsers get enhancements */
@supports (display: grid) {
  .dashboard-grid {
    display: grid;
    gap: 1rem;
    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  }
}

/* Fallback for older browsers */
.dashboard-grid {
  display: flex;
  flex-wrap: wrap;
  margin: -0.5rem;
}

.dashboard-grid > * {
  flex: 1 1 300px;
  margin: 0.5rem;
}
```

### 7.4 Testing Checklist

```yaml
Visual Testing:
  - Light theme appearance
  - Dark theme appearance
  - High contrast mode
  - Print styles
  - Mobile responsiveness
  - Tablet responsiveness
  - Desktop layouts

Interaction Testing:
  - Mouse interactions
  - Keyboard navigation
  - Touch interactions
  - Screen reader navigation
  - Voice control compatibility

Performance Testing:
  - Page load time < 3s
  - First paint < 1.5s
  - Interactive time < 3s
  - Smooth animations (60fps)
  - Memory usage < 50MB

Accessibility Testing:
  - WCAG 2.1 AA compliance
  - Keyboard navigation complete
  - Screen reader announcement
  - Color contrast ratios
  - Focus indicators visible
  - Error identification clear

Browser Testing:
  - Chrome (latest 2 versions)
  - Firefox (latest 2 versions)
  - Safari (latest 2 versions)
  - Edge (latest 2 versions)
  - Mobile browsers
```

## Summary

This frontend design system provides a complete foundation for building the Screen-to-RTSP ONVIF Bridge interface. Key features include:

1. **Comprehensive Visual System**: Complete color palette, typography, spacing, and component library
2. **Dual Interface Design**: Native system tray and web configuration interface
3. **Accessibility First**: WCAG 2.1 AA compliance with full keyboard and screen reader support
4. **Performance Optimized**: Lightweight implementation with lazy loading and efficient animations
5. **No Framework Dependencies**: Pure HTML/CSS/JavaScript for minimal footprint
6. **Cross-Platform Consistency**: Unified experience across Windows, macOS, and Linux

The design prioritizes simplicity, accessibility, and performance while providing a professional and modern user experience.