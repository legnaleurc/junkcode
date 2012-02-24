#import "inspectorprivate.h"
#import "UIElementUtilities.h"

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

#import <QtGui/QApplication>
#import <QtGui/QDesktopWidget>

#import <QtCore/QtDebug>

const int MIN_SIZE = 8;

AXUIElementRef lineageOfUIElement( AXUIElementRef element ) {
	AXUIElementRef parent = [UIElementUtilities parentOfUIElement:element];
	while( parent ) {
		NSString * roleName = [UIElementUtilities roleOfUIElement:element];
		if( [roleName isEqualToString:@"AXMenuBar"] || [roleName isEqualToString:@"AXWindow"] ) {
			return element;
		}
		element = parent;
		parent = [UIElementUtilities parentOfUIElement:element];
	}
	return element;
}

NSArray * subelementsFromElement( AXUIElementRef element ) {
	NSArray * subElements = nil;
	CFIndex count = 0;
	AXError result;
	NSString * attribute = @"AXChildren";

	result = AXUIElementGetAttributeValueCount( element, ( CFStringRef )attribute, &count );
	if( result != kAXErrorSuccess ) {
		return nil;
	}
	result = AXUIElementCopyAttributeValues( element, ( CFStringRef )attribute, 0, count, ( CFArrayRef * )&subElements );
	if( result != kAXErrorSuccess ) {
		return nil;
	}

	return [subElements autorelease];
}

QRect rectOfElement( AXUIElementRef element ) {
	NSRect bounds = [UIElementUtilities frameOfUIElement:element];
	// NOTE y position is count from bottom in Cocoa
	int realY = QApplication::desktop()->screenGeometry().height() - bounds.origin.y - bounds.size.height;
	return QRect( bounds.origin.x, realY, bounds.size.width, bounds.size.height );
}

void getWindows( std::vector< QRect > & windows, AXUIElementRef element ) {
	NSArray * children = subelementsFromElement( element );
	NSEnumerator * e = [children objectEnumerator];
	AXUIElementRef child;
	while( child = ( AXUIElementRef )[e nextObject] ) {
		QRect r( rectOfElement( child ) );
		if( r.width() < MIN_SIZE || r.height() < MIN_SIZE ) {
			continue;
		}
		windows.push_back( r );
		getWindows( windows, child );
	}
}

bool operator< ( const QRect& r1, const QRect& r2 ) {
	return r1.width() * r1.height() < r2.width() * r2.height();
}

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
		this->p_->setCurrentUIElement( lineageOfUIElement( newElement ) );
		QRect r( rectOfElement( this->p_->getCurrentUIElement() ) );
		windows.push_back( r );
		getWindows( windows, this->p_->getCurrentUIElement() );
		std::sort( windows.begin(), windows.end() );
		if ( r.isNull() ) {
			return QPixmap();
		}
		return QPixmap::grabWindow( QApplication::desktop()->winId(), r.x(), r.y(), r.width(), r.height() );
	}

	this->p_->lastMousePoint = cocoaPoint;
	return QPixmap();
}
