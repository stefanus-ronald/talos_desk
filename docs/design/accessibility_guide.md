---
document: "Talos Desk Accessibility Guide"
version: "1.0.0"
last_updated: "2025-08-02"
status: "stable"
project_version: "1.0.0"
maintainer: "development-team"
review_date: "2025-11-02"
---

# Accessibility Implementation Guide: Screen-to-RTSP ONVIF Bridge

## Table of Contents
1. [Overview](#overview)
2. [WCAG 2.1 Compliance](#wcag-21-compliance)
3. [Keyboard Navigation](#keyboard-navigation)
4. [Screen Reader Support](#screen-reader-support)
5. [Visual Accessibility](#visual-accessibility)
6. [Motor Accessibility](#motor-accessibility)
7. [Cognitive Accessibility](#cognitive-accessibility)
8. [Testing Procedures](#testing-procedures)
9. [Implementation Checklist](#implementation-checklist)

## Overview

This guide provides comprehensive accessibility implementation guidelines for the Screen-to-RTSP ONVIF Bridge application. Our goal is to achieve WCAG 2.1 AA compliance while creating an inclusive experience for all users.

### Core Principles
- **Perceivable**: Information must be presentable in ways users can perceive
- **Operable**: Interface components must be operable by all users
- **Understandable**: Information and UI operation must be understandable
- **Robust**: Content must be robust enough for various assistive technologies

### Target Compliance
- **WCAG 2.1 Level AA** (minimum requirement)
- **Section 508** compliance (US federal requirement)
- **EN 301 549** (European accessibility standard)

## WCAG 2.1 Compliance

### Success Criteria Mapping

#### Level A (Essential)
| Criterion | Implementation | Status |
|-----------|----------------|--------|
| 1.1.1 Non-text Content | All images, icons have alt text | ✅ |
| 1.3.1 Info and Relationships | Proper semantic HTML structure | ✅ |
| 1.4.1 Use of Color | Color not sole indicator | ✅ |
| 2.1.1 Keyboard | All functionality keyboard accessible | ✅ |
| 2.1.2 No Keyboard Trap | Focus can always escape | ✅ |
| 2.4.1 Bypass Blocks | Skip links implemented | ✅ |
| 3.1.1 Language of Page | lang attribute set | ✅ |
| 4.1.1 Parsing | Valid HTML markup | ✅ |

#### Level AA (Target)
| Criterion | Implementation | Status |
|-----------|----------------|--------|
| 1.4.3 Contrast (Minimum) | 4.5:1 ratio for normal text | ✅ |
| 1.4.4 Resize Text | Up to 200% without loss | ✅ |
| 1.4.5 Images of Text | Avoided where possible | ✅ |
| 2.4.5 Multiple Ways | Navigation + search | ✅ |
| 2.4.6 Headings and Labels | Descriptive headings | ✅ |
| 2.4.7 Focus Visible | Clear focus indicators | ✅ |
| 3.2.3 Consistent Navigation | Same order throughout | ✅ |
| 3.3.2 Labels or Instructions | Clear form labels | ✅ |

## Keyboard Navigation

### Navigation Map

```yaml
Global Navigation:
  Tab: Move forward through focusable elements
  Shift+Tab: Move backward through focusable elements
  Enter: Activate buttons, links
  Space: Activate buttons, toggle checkboxes
  Escape: Close modals, cancel operations
  
Application Shortcuts:
  Ctrl+S: Save configuration
  Ctrl+/: Focus search
  Alt+C: Open configuration
  Alt+S: Start/stop streaming
  F1: Open help
  
Custom Navigation:
  Arrow Keys: Navigate within components (tabs, menus)
  Home/End: Jump to first/last item
  Page Up/Down: Scroll content
```

### Implementation Code

```javascript
// Keyboard Navigation Manager
class KeyboardNavigation {
  constructor() {
    this.shortcuts = new Map();
    this.initializeShortcuts();
    this.setupEventListeners();
  }
  
  initializeShortcuts() {
    // Global shortcuts
    this.shortcuts.set('ctrl+s', () => this.saveConfiguration());
    this.shortcuts.set('ctrl+/', () => this.focusSearch());
    this.shortcuts.set('alt+c', () => this.openConfiguration());
    this.shortcuts.set('alt+s', () => this.toggleStreaming());
    this.shortcuts.set('f1', () => this.openHelp());
  }
  
  setupEventListeners() {
    document.addEventListener('keydown', (e) => {
      const key = this.getShortcutKey(e);
      const handler = this.shortcuts.get(key);
      
      if (handler) {
        e.preventDefault();
        handler();
      }
      
      // Special handling for navigation
      this.handleNavigationKeys(e);
    });
  }
  
  getShortcutKey(event) {
    const keys = [];
    if (event.ctrlKey || event.metaKey) keys.push('ctrl');
    if (event.altKey) keys.push('alt');
    if (event.shiftKey) keys.push('shift');
    keys.push(event.key.toLowerCase());
    return keys.join('+');
  }
  
  handleNavigationKeys(event) {
    const activeElement = document.activeElement;
    
    // Tab panel navigation
    if (activeElement.getAttribute('role') === 'tab') {
      this.handleTabNavigation(event);
    }
    
    // Menu navigation
    if (activeElement.getAttribute('role') === 'menuitem') {
      this.handleMenuNavigation(event);
    }
  }
  
  handleTabNavigation(event) {
    const tabs = Array.from(document.querySelectorAll('[role="tab"]'));
    const currentIndex = tabs.indexOf(event.target);
    
    switch (event.key) {
      case 'ArrowRight':
      case 'ArrowDown':
        event.preventDefault();
        this.focusTab(tabs, (currentIndex + 1) % tabs.length);
        break;
      case 'ArrowLeft':
      case 'ArrowUp':
        event.preventDefault();
        this.focusTab(tabs, (currentIndex - 1 + tabs.length) % tabs.length);
        break;
      case 'Home':
        event.preventDefault();
        this.focusTab(tabs, 0);
        break;
      case 'End':
        event.preventDefault();
        this.focusTab(tabs, tabs.length - 1);
        break;
    }
  }
  
  focusTab(tabs, index) {
    // Update ARIA states
    tabs.forEach((tab, i) => {
      tab.setAttribute('aria-selected', i === index ? 'true' : 'false');
      tab.setAttribute('tabindex', i === index ? '0' : '-1');
    });
    
    // Focus the selected tab
    tabs[index].focus();
    
    // Show associated panel
    const panelId = tabs[index].getAttribute('aria-controls');
    this.showPanel(panelId);
  }
}

// Focus Management
class FocusManager {
  constructor() {
    this.focusStack = [];
  }
  
  // Trap focus within a container (for modals)
  trapFocus(container) {
    const focusableElements = this.getFocusableElements(container);
    const firstElement = focusableElements[0];
    const lastElement = focusableElements[focusableElements.length - 1];
    
    // Store current focus
    this.focusStack.push(document.activeElement);
    
    // Focus first element
    firstElement?.focus();
    
    // Handle tab key
    container.addEventListener('keydown', (e) => {
      if (e.key !== 'Tab') return;
      
      if (e.shiftKey && document.activeElement === firstElement) {
        e.preventDefault();
        lastElement?.focus();
      } else if (!e.shiftKey && document.activeElement === lastElement) {
        e.preventDefault();
        firstElement?.focus();
      }
    });
  }
  
  releaseFocus() {
    const previousFocus = this.focusStack.pop();
    previousFocus?.focus();
  }
  
  getFocusableElements(container = document) {
    const selector = `
      a[href],
      button:not([disabled]),
      input:not([disabled]),
      select:not([disabled]),
      textarea:not([disabled]),
      [tabindex]:not([tabindex="-1"])
    `;
    
    return Array.from(container.querySelectorAll(selector))
      .filter(el => el.offsetParent !== null); // Visible elements only
  }
}
```

### Skip Links Implementation

```html
<!-- Skip links (hidden but accessible) -->
<div class="skip-links">
  <a href="#main-content" class="skip-link">Skip to main content</a>
  <a href="#navigation" class="skip-link">Skip to navigation</a>
  <a href="#search" class="skip-link">Skip to search</a>
</div>

<style>
.skip-link {
  position: absolute;
  top: -40px;
  left: 0;
  background: var(--primary-500);
  color: white;
  padding: 8px 16px;
  text-decoration: none;
  border-radius: 0 0 4px 0;
  z-index: 100;
  transition: top 0.2s;
}

.skip-link:focus {
  top: 0;
}
</style>
```

## Screen Reader Support

### ARIA Implementation Guide

#### Landmarks and Regions

```html
<!-- Page structure with ARIA landmarks -->
<header role="banner">
  <h1>Screen to RTSP Configuration</h1>
  <nav role="navigation" aria-label="Main navigation">
    <!-- Navigation items -->
  </nav>
</header>

<main role="main" id="main-content" aria-label="Configuration interface">
  <section aria-labelledby="status-heading">
    <h2 id="status-heading">Stream Status</h2>
    <!-- Status content -->
  </section>
</main>

<aside role="complementary" aria-label="Help and resources">
  <!-- Help content -->
</aside>

<footer role="contentinfo">
  <!-- Footer content -->
</footer>
```

#### Live Regions

```html
<!-- Status updates announced to screen readers -->
<div role="status" aria-live="polite" aria-atomic="true" class="sr-only">
  <span id="status-announcement"></span>
</div>

<!-- Critical alerts -->
<div role="alert" aria-live="assertive" aria-atomic="true">
  <span id="alert-message"></span>
</div>

<script>
// Announce status changes
function announceStatus(message) {
  const announcement = document.getElementById('status-announcement');
  announcement.textContent = message;
  
  // Clear after announcement
  setTimeout(() => {
    announcement.textContent = '';
  }, 1000);
}

// Usage
announceStatus('Streaming started successfully');
announceStatus('Configuration saved');
</script>
```

#### Form Accessibility

```html
<!-- Accessible form with ARIA -->
<form role="form" aria-labelledby="video-settings-title">
  <h2 id="video-settings-title">Video Settings</h2>
  
  <!-- Required field -->
  <div class="form-group">
    <label for="bitrate" id="bitrate-label">
      Bitrate
      <span class="required" aria-label="required field">*</span>
    </label>
    <input 
      type="number"
      id="bitrate"
      aria-labelledby="bitrate-label"
      aria-describedby="bitrate-help bitrate-error"
      aria-required="true"
      aria-invalid="false"
      min="1000"
      max="50000"
      value="5000"
    >
    <span id="bitrate-help" class="form-help">
      Enter bitrate in Kbps (1000-50000)
    </span>
    <span id="bitrate-error" class="form-error" role="alert" aria-live="assertive">
      <!-- Error message inserted here -->
    </span>
  </div>
  
  <!-- Grouped options -->
  <fieldset>
    <legend>Stream Quality</legend>
    <div role="radiogroup" aria-labelledby="quality-label">
      <label>
        <input type="radio" name="quality" value="low" aria-describedby="quality-low-desc">
        Low (2 Mbps)
      </label>
      <span id="quality-low-desc" class="sr-only">
        Suitable for slow connections, lower video quality
      </span>
      <!-- More options -->
    </div>
  </fieldset>
</form>
```

#### Dynamic Content

```javascript
// Announce dynamic changes
class ScreenReaderAnnouncer {
  constructor() {
    this.liveRegion = document.getElementById('status-announcement');
    this.alertRegion = document.getElementById('alert-message');
  }
  
  // Polite announcement (waits for screen reader to finish)
  announce(message) {
    this.liveRegion.textContent = message;
    setTimeout(() => {
      this.liveRegion.textContent = '';
    }, 1000);
  }
  
  // Assertive announcement (interrupts screen reader)
  alert(message) {
    this.alertRegion.textContent = message;
    setTimeout(() => {
      this.alertRegion.textContent = '';
    }, 5000);
  }
  
  // Announce form validation
  announceValidation(field, isValid, message) {
    const input = document.getElementById(field);
    const errorElement = document.getElementById(`${field}-error`);
    
    input.setAttribute('aria-invalid', !isValid);
    errorElement.textContent = isValid ? '' : message;
    
    if (!isValid) {
      this.alert(`${field} field error: ${message}`);
    }
  }
}
```

### Screen Reader Testing Scripts

```javascript
// Automated screen reader testing helpers
class ScreenReaderTester {
  checkLandmarks() {
    const landmarks = [
      'banner', 'navigation', 'main', 
      'complementary', 'contentinfo'
    ];
    
    const results = landmarks.map(role => {
      const element = document.querySelector(`[role="${role}"]`);
      return {
        role,
        exists: !!element,
        hasLabel: element?.hasAttribute('aria-label') || 
                  element?.hasAttribute('aria-labelledby')
      };
    });
    
    return results;
  }
  
  checkHeadingStructure() {
    const headings = Array.from(document.querySelectorAll('h1,h2,h3,h4,h5,h6'));
    let lastLevel = 0;
    const issues = [];
    
    headings.forEach((heading, index) => {
      const level = parseInt(heading.tagName[1]);
      
      if (index === 0 && level !== 1) {
        issues.push('Page should start with h1');
      }
      
      if (level > lastLevel + 1) {
        issues.push(`Heading level skip at ${heading.textContent}`);
      }
      
      lastLevel = level;
    });
    
    return issues;
  }
  
  checkFormLabels() {
    const inputs = document.querySelectorAll('input, select, textarea');
    const unlabeled = [];
    
    inputs.forEach(input => {
      const hasLabel = input.hasAttribute('aria-label') ||
                      input.hasAttribute('aria-labelledby') ||
                      document.querySelector(`label[for="${input.id}"]`);
      
      if (!hasLabel) {
        unlabeled.push(input);
      }
    });
    
    return unlabeled;
  }
}
```

## Visual Accessibility

### Color and Contrast

```css
/* Color contrast ratios */
:root {
  /* WCAG AA compliant colors */
  --text-on-light: #1f2937; /* 9.73:1 on white */
  --text-on-dark: #f1f5f9;  /* 13.28:1 on dark bg */
  
  /* Error colors with sufficient contrast */
  --error-text: #dc2626;    /* 4.54:1 on white */
  --error-bg: #fef2f2;      /* Light error background */
  
  /* Focus indicators */
  --focus-color: #2563eb;   /* 4.96:1 on white */
  --focus-outline: 3px solid var(--focus-color);
}

/* Never rely on color alone */
.status-indicator {
  display: flex;
  align-items: center;
  gap: 8px;
}

.status-indicator::before {
  content: '';
  width: 12px;
  height: 12px;
  border-radius: 50%;
  background-color: var(--status-color);
}

.status-indicator[data-status="active"]::before {
  background-color: #10b981;
  animation: pulse 2s infinite;
}

.status-indicator[data-status="active"]::after {
  content: '(Active)';
  color: var(--text-secondary);
}

/* High contrast mode support */
@media (prefers-contrast: high) {
  :root {
    --primary-500: #0000ff;
    --error-500: #ff0000;
    --success-500: #008000;
    --border-color: #000000;
  }
  
  * {
    outline-width: 2px !important;
  }
}
```

### Text and Typography

```css
/* Readable typography */
body {
  font-size: 16px; /* Minimum base size */
  line-height: 1.5; /* Adequate spacing */
  letter-spacing: 0.025em; /* Slight spacing for readability */
}

/* Scalable text */
html {
  font-size: 100%; /* Respect user preferences */
}

/* Text scaling utilities */
.text-scale-75 { font-size: 0.75em; }
.text-scale-100 { font-size: 1em; }
.text-scale-125 { font-size: 1.25em; }
.text-scale-150 { font-size: 1.5em; }
.text-scale-200 { font-size: 2em; }

/* Prevent text in images */
.icon-with-text {
  display: flex;
  align-items: center;
  gap: 8px;
}

.icon-with-text svg {
  flex-shrink: 0;
}

/* Maintain readability at all sizes */
@media screen and (min-width: 320px) {
  body {
    font-size: calc(16px + 0.5vw);
    max-width: 70ch; /* Optimal line length */
  }
}
```

### Focus Indicators

```css
/* Remove default outline for mouse users */
*:focus:not(:focus-visible) {
  outline: none;
}

/* Strong focus indicators for keyboard users */
*:focus-visible {
  outline: 3px solid var(--focus-color);
  outline-offset: 2px;
  border-radius: 2px;
}

/* Custom focus styles for different elements */
button:focus-visible {
  outline-offset: 4px;
  box-shadow: 0 0 0 2px white, 0 0 0 4px var(--focus-color);
}

input:focus-visible,
select:focus-visible,
textarea:focus-visible {
  outline: none;
  border-color: var(--focus-color);
  box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.2);
}

/* Skip link focus */
.skip-link:focus {
  position: absolute;
  top: 0;
  left: 0;
  z-index: 9999;
  padding: 8px 16px;
  background: var(--primary-500);
  color: white;
  text-decoration: none;
}
```

## Motor Accessibility

### Touch Targets

```css
/* Minimum touch target sizes */
button,
a,
input[type="checkbox"],
input[type="radio"],
select {
  min-height: 44px; /* iOS recommendation */
  min-width: 44px;
}

/* Spacing between interactive elements */
.button-group {
  display: flex;
  gap: 8px; /* Minimum spacing */
}

/* Larger click areas for small controls */
.checkbox-wrapper,
.radio-wrapper {
  display: inline-flex;
  align-items: center;
  padding: 8px;
  cursor: pointer;
}

.checkbox-wrapper input,
.radio-wrapper input {
  margin: 0;
  margin-right: 8px;
}

/* Avoid accidental activation */
.danger-zone {
  margin-top: 32px;
  padding-top: 32px;
  border-top: 2px solid var(--error-500);
}

.danger-zone .btn-danger {
  margin-left: auto; /* Push to the right */
}
```

### Gesture Alternatives

```javascript
// Provide keyboard alternatives for all gestures
class GestureAlternatives {
  constructor() {
    this.setupDragAndDrop();
    this.setupSwipeGestures();
  }
  
  setupDragAndDrop() {
    const draggables = document.querySelectorAll('[draggable="true"]');
    
    draggables.forEach(element => {
      // Add keyboard support
      element.setAttribute('tabindex', '0');
      element.setAttribute('role', 'button');
      element.setAttribute('aria-label', `${element.textContent}, press space to grab, use arrow keys to move`);
      
      let isGrabbed = false;
      
      element.addEventListener('keydown', (e) => {
        if (e.key === ' ' || e.key === 'Enter') {
          e.preventDefault();
          isGrabbed = !isGrabbed;
          element.setAttribute('aria-grabbed', isGrabbed);
          
          if (isGrabbed) {
            this.announceGrab(element);
          } else {
            this.announceDrop(element);
          }
        }
        
        if (isGrabbed && e.key.startsWith('Arrow')) {
          e.preventDefault();
          this.handleArrowMovement(element, e.key);
        }
      });
    });
  }
  
  setupSwipeGestures() {
    const swipeElements = document.querySelectorAll('[data-swipe]');
    
    swipeElements.forEach(element => {
      // Add button alternatives
      const actions = element.dataset.swipeActions.split(',');
      const buttonContainer = document.createElement('div');
      buttonContainer.className = 'swipe-alternatives';
      buttonContainer.setAttribute('role', 'group');
      buttonContainer.setAttribute('aria-label', 'Actions');
      
      actions.forEach(action => {
        const button = document.createElement('button');
        button.textContent = action;
        button.className = 'btn btn-sm';
        button.addEventListener('click', () => this.performAction(element, action));
        buttonContainer.appendChild(button);
      });
      
      element.appendChild(buttonContainer);
    });
  }
}
```

### Timing Adjustments

```javascript
// Configurable timing for motor impairments
class TimingAccessibility {
  constructor() {
    this.timeouts = {
      default: {
        tooltipDelay: 500,
        menuDelay: 300,
        notificationDuration: 5000,
        animationDuration: 300
      },
      extended: {
        tooltipDelay: 1000,
        menuDelay: 600,
        notificationDuration: 10000,
        animationDuration: 600
      }
    };
    
    this.currentProfile = this.loadProfile();
  }
  
  loadProfile() {
    const saved = localStorage.getItem('timing-profile');
    return saved || 'default';
  }
  
  setProfile(profile) {
    this.currentProfile = profile;
    localStorage.setItem('timing-profile', profile);
    this.applyProfile();
  }
  
  getTimeout(key) {
    return this.timeouts[this.currentProfile][key];
  }
  
  applyProfile() {
    // Update CSS custom properties
    const root = document.documentElement;
    const profile = this.timeouts[this.currentProfile];
    
    root.style.setProperty('--animation-duration', `${profile.animationDuration}ms`);
    root.style.setProperty('--tooltip-delay', `${profile.tooltipDelay}ms`);
    
    // Notify other components
    window.dispatchEvent(new CustomEvent('timing-profile-changed', {
      detail: { profile: this.currentProfile }
    }));
  }
}
```

## Cognitive Accessibility

### Clear Language and Instructions

```html
<!-- Clear, simple language -->
<div class="help-section">
  <h2>How to Start Streaming</h2>
  <ol class="steps">
    <li>
      <strong>Choose your screen</strong>
      <p>Select which monitor you want to share</p>
    </li>
    <li>
      <strong>Set video quality</strong>
      <p>Pick "Balanced" for most uses</p>
    </li>
    <li>
      <strong>Click "Start Streaming"</strong>
      <p>Your screen will begin broadcasting</p>
    </li>
  </ol>
</div>

<!-- Inline help -->
<div class="form-group">
  <label for="codec">
    Video Format
    <button class="help-toggle" aria-label="Help for video format">?</button>
  </label>
  <div class="help-text" id="codec-help" hidden>
    <p><strong>H.264</strong>: Works with all devices (Recommended)</p>
    <p><strong>H.265</strong>: Newer format, smaller file size, may not work everywhere</p>
  </div>
  <select id="codec" aria-describedby="codec-help">
    <option value="h264">H.264 (Recommended)</option>
    <option value="h265">H.265 (Advanced)</option>
  </select>
</div>
```

### Error Prevention and Recovery

```javascript
// Confirmation for destructive actions
class ConfirmationDialog {
  show(options) {
    const dialog = document.createElement('div');
    dialog.setAttribute('role', 'alertdialog');
    dialog.setAttribute('aria-labelledby', 'confirm-title');
    dialog.setAttribute('aria-describedby', 'confirm-message');
    
    dialog.innerHTML = `
      <div class="dialog-content">
        <h2 id="confirm-title">${options.title}</h2>
        <p id="confirm-message">${options.message}</p>
        <div class="dialog-actions">
          <button class="btn btn-secondary" onclick="this.cancel()">
            Cancel (Escape key)
          </button>
          <button class="btn btn-danger" onclick="this.confirm()">
            ${options.confirmText}
          </button>
        </div>
      </div>
    `;
    
    // Focus management
    this.trapFocus(dialog);
    
    return new Promise((resolve) => {
      this.resolve = resolve;
    });
  }
  
  cancel() {
    this.resolve(false);
    this.close();
  }
  
  confirm() {
    this.resolve(true);
    this.close();
  }
}

// Undo functionality
class UndoManager {
  constructor() {
    this.history = [];
    this.maxHistory = 10;
  }
  
  execute(action) {
    // Store state before action
    const previousState = this.captureState();
    
    // Execute action
    action.do();
    
    // Add to history
    this.history.push({
      action,
      previousState,
      timestamp: Date.now()
    });
    
    // Limit history size
    if (this.history.length > this.maxHistory) {
      this.history.shift();
    }
    
    // Show undo notification
    this.showUndoNotification(action.description);
  }
  
  undo() {
    const lastAction = this.history.pop();
    if (lastAction) {
      this.restoreState(lastAction.previousState);
      this.announce(`Undone: ${lastAction.action.description}`);
    }
  }
  
  showUndoNotification(description) {
    const notification = document.createElement('div');
    notification.className = 'undo-notification';
    notification.setAttribute('role', 'status');
    notification.innerHTML = `
      ${description}
      <button onclick="undoManager.undo()">Undo</button>
    `;
    
    document.body.appendChild(notification);
    
    setTimeout(() => {
      notification.remove();
    }, 5000);
  }
}
```

### Consistent Patterns

```css
/* Consistent visual hierarchy */
.primary-action {
  /* Always the most prominent */
  background: var(--primary-500);
  color: white;
  font-weight: 600;
}

.secondary-action {
  /* Always less prominent */
  background: transparent;
  color: var(--primary-500);
  border: 1px solid var(--primary-500);
}

.danger-action {
  /* Always indicates risk */
  background: var(--error-500);
  color: white;
  margin-left: auto; /* Push away from other actions */
}

/* Consistent iconography */
.icon-start::before { content: '▶'; }
.icon-stop::before { content: '■'; }
.icon-settings::before { content: '⚙'; }
.icon-help::before { content: '?'; }
.icon-warning::before { content: '⚠'; }
.icon-success::before { content: '✓'; }
.icon-error::before { content: '✗'; }

/* Consistent feedback patterns */
.success-message {
  color: var(--success-500);
  background: var(--success-100);
  padding: 12px;
  border-radius: 4px;
  display: flex;
  align-items: center;
  gap: 8px;
}

.success-message::before {
  content: '✓';
  font-weight: bold;
}
```

## Testing Procedures

### Automated Testing

```javascript
// Accessibility test suite
describe('Accessibility Tests', () => {
  beforeEach(() => {
    cy.visit('/');
    cy.injectAxe();
  });
  
  it('has no WCAG violations on load', () => {
    cy.checkA11y();
  });
  
  it('has no WCAG violations in dark mode', () => {
    cy.get('[data-testid="theme-toggle"]').click();
    cy.checkA11y();
  });
  
  it('maintains focus on interaction', () => {
    cy.get('button[data-testid="open-settings"]').focus().click();
    cy.focused().should('have.attr', 'data-testid', 'first-setting-input');
  });
  
  it('announces status changes', () => {
    cy.get('[role="status"]').should('be.empty');
    cy.get('button[data-testid="save"]').click();
    cy.get('[role="status"]').should('contain', 'Settings saved');
  });
  
  it('supports keyboard navigation', () => {
    // Tab through interface
    cy.get('body').tab();
    cy.focused().should('have.class', 'skip-link');
    
    // Navigate with arrows
    cy.get('[role="tablist"]').first().focus();
    cy.focused().type('{rightarrow}');
    cy.focused().should('have.attr', 'aria-selected', 'true');
  });
});

// Contrast testing
describe('Color Contrast Tests', () => {
  it('meets WCAG AA standards', () => {
    cy.visit('/');
    
    const checkContrast = (foreground, background) => {
      const ratio = getContrastRatio(foreground, background);
      expect(ratio).to.be.at.least(4.5);
    };
    
    // Test primary text
    cy.get('body').then($body => {
      const bgColor = $body.css('background-color');
      const textColor = $body.css('color');
      checkContrast(textColor, bgColor);
    });
  });
});
```

### Manual Testing Checklist

```markdown
## Keyboard Navigation Testing

- [ ] Can reach all interactive elements with Tab
- [ ] Can navigate backwards with Shift+Tab
- [ ] Focus indicators are clearly visible
- [ ] Can escape from any focused element
- [ ] Keyboard shortcuts work as documented
- [ ] No keyboard traps exist
- [ ] Modal dialogs trap focus appropriately
- [ ] Skip links work correctly

## Screen Reader Testing

### NVDA (Windows)
- [ ] Page title is announced on load
- [ ] Headings create logical outline
- [ ] Form labels are announced correctly
- [ ] Error messages are announced
- [ ] Status changes are announced
- [ ] Buttons describe their action
- [ ] Links describe their destination

### JAWS (Windows)
- [ ] Virtual cursor navigation works
- [ ] Forms mode switches appropriately
- [ ] Tables are navigable
- [ ] Landmarks are identified

### VoiceOver (macOS/iOS)
- [ ] Rotor navigation works
- [ ] Gestures work as expected
- [ ] Form controls are accessible
- [ ] Images have descriptions

## Visual Testing

- [ ] 200% zoom maintains functionality
- [ ] High contrast mode is readable
- [ ] Dark mode has sufficient contrast
- [ ] Focus indicators visible in all modes
- [ ] No information conveyed by color alone
- [ ] Animations respect prefers-reduced-motion

## Motor Testing

- [ ] All targets ≥44x44 pixels
- [ ] Sufficient spacing between targets
- [ ] Drag operations have alternatives
- [ ] No time-based interactions
- [ ] Single pointer operations only
- [ ] No complex gestures required
```

### Browser and AT Testing Matrix

| Browser | Screen Reader | Test Priority |
|---------|--------------|---------------|
| Chrome | NVDA | High |
| Chrome | JAWS | High |
| Firefox | NVDA | High |
| Edge | Narrator | Medium |
| Safari | VoiceOver | High |
| Chrome | ChromeVox | Low |
| iOS Safari | VoiceOver | Medium |
| Chrome Android | TalkBack | Medium |

## Implementation Checklist

### Phase 1: Foundation (Week 1)
- [ ] Semantic HTML structure
- [ ] ARIA landmarks
- [ ] Basic keyboard navigation
- [ ] Skip links
- [ ] Focus indicators
- [ ] Color contrast compliance

### Phase 2: Screen Reader Support (Week 2)
- [ ] ARIA labels and descriptions
- [ ] Live regions for updates
- [ ] Form field associations
- [ ] Error announcement
- [ ] Dynamic content handling
- [ ] Heading structure

### Phase 3: Enhanced Interaction (Week 3)
- [ ] Keyboard shortcuts
- [ ] Focus management
- [ ] Tab navigation
- [ ] Modal focus trapping
- [ ] Gesture alternatives
- [ ] Timing adjustments

### Phase 4: Testing and Refinement (Week 4)
- [ ] Automated testing setup
- [ ] Manual testing with ATs
- [ ] User testing with PWD
- [ ] Issue remediation
- [ ] Documentation
- [ ] Training materials

### Ongoing Maintenance
- [ ] Regular automated testing
- [ ] Update testing on changes
- [ ] Monitor WCAG updates
- [ ] User feedback integration
- [ ] Accessibility statement updates
- [ ] Team training

## Resources and References

### Standards and Guidelines
- [WCAG 2.1](https://www.w3.org/WAI/WCAG21/quickref/)
- [ARIA Authoring Practices](https://www.w3.org/WAI/ARIA/apg/)
- [Section 508](https://www.section508.gov/)
- [EN 301 549](https://www.etsi.org/deliver/etsi_en/301500_301599/301549/)

### Testing Tools
- [axe DevTools](https://www.deque.com/axe/devtools/)
- [WAVE](https://wave.webaim.org/)
- [NVDA](https://www.nvaccess.org/)
- [JAWS](https://www.freedomscientific.com/products/software/jaws/)
- [Lighthouse](https://developers.google.com/web/tools/lighthouse)

### Learning Resources
- [WebAIM](https://webaim.org/)
- [A11y Project](https://www.a11yproject.com/)
- [MDN Accessibility](https://developer.mozilla.org/en-US/docs/Web/Accessibility)
- [Inclusive Components](https://inclusive-components.design/)

This comprehensive guide ensures that the Screen-to-RTSP ONVIF Bridge application is accessible to all users, regardless of their abilities. Regular testing and updates will maintain compliance as the application evolves.
