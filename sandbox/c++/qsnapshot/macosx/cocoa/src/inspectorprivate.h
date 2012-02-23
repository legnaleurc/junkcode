#import "inspector.hpp"
#import <AppKit/NSAccessibility.h>

class Inspector::Private {
public:
	explicit Private( Inspector * host );
	~Private();

	AXUIElementRef getCurrentUIElement() const;
	void setCurrentUIElement( AXUIElementRef uiElement );
	void updateUIElementInfoWithAnimation( BOOL flag );

	Inspector * host;
	AXUIElementRef systemWideElement;
	AXUIElementRef currentUIElement;
	NSPoint lastMousePoint;
};
