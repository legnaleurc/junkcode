#import "inspectorprivate.h"
#import "UIElementUtilities.h"

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

Inspector::Private::Private( Inspector * host ):
host( host ),
systemWideElement( AXUIElementCreateSystemWide() ),
currentUIElement( NULL ),
lastMousePoint() {
}

Inspector::Private::~Private() {
	if( this->systemWideElement ) {
		CFRelease( this->systemWideElement );
	}
}

AXUIElementRef Inspector::Private::getCurrentUIElement() const {
	return this->currentUIElement;
}

void Inspector::Private::setCurrentUIElement( AXUIElementRef uiElement ) {
	[(id) this->currentUIElement autorelease];
	this->currentUIElement = (AXUIElementRef)[(id) uiElement retain];
}

Inspector::Inspector():
p_( new Private( this ) ) {
}

QPixmap Inspector::grabWindow( std::vector< QRect > & windows ) {
	NSPoint cocoaPoint = [NSEvent mouseLocation];
	CGPoint pointAsCGPoint = [UIElementUtilities carbonScreenPointFromCocoaScreenPoint:cocoaPoint];
	AXUIElementRef newElement = NULL;

	// Ask Accessibility API for UI Element under the mouse
	// And update the display if a different UIElement
	AXError result = AXUIElementCopyElementAtPosition( this->p_->systemWideElement, pointAsCGPoint.x, pointAsCGPoint.y, &newElement );
	if( result == kAXErrorSuccess && newElement && ( this->p_->getCurrentUIElement() == NULL || ! CFEqual( this->p_->getCurrentUIElement(), newElement ) ) ) {
		this->p_->setCurrentUIElement( newElement );
//		this->p_->updateUIElementInfoWithAnimation( NO );
	}

	this->p_->lastMousePoint = cocoaPoint;
	return QPixmap();
}
