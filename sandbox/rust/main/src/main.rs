mod qt_gui;


fn main () -> () {
    let app = qt_gui::QApplication::new();

    let label = qt_gui::QLabel::new();
    label.resize(640, 480);
    label.show();

    app.exec();
}
