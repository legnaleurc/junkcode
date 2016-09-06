mod QtGui;


fn main () -> () {
    let app = QtGui::QApplication::new();

    let label = QtGui::QLabel::new();
    label.resize(640, 480);
    label.show();

    app.exec();
}
