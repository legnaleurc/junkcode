extern crate libc;

use std::ffi::CString;
use libc::c_int;
use libc::c_char;


struct RustQApplication;
struct RustQWidget;
struct RustQLabel;


#[link(name="qt5rust")]
extern {
    fn qapplication_new(argc: c_int, argv: *mut *mut c_char) -> *mut RustQApplication;
    fn qapplication_exec(this: *mut RustQApplication) -> c_int;

    fn qlabel_new(parent: *mut RustQWidget) -> *mut RustQLabel;
    fn qlabel_resize(this: *mut RustQLabel, width: c_int, height: c_int);
    fn qlabel_show(this: *mut RustQLabel) -> ();
}


fn main () -> () {
    let args = std::env::args().map(|arg| CString::new(arg).unwrap() ).collect::<Vec<CString>>();
    let c_args = args.iter().map(|arg| arg.as_ptr()).collect::<Vec<*const c_char>>();
    unsafe {
        let app = qapplication_new(c_args.len() as c_int, c_args.as_ptr() as *mut *mut c_char);
        let label = qlabel_new(std::ptr::null_mut());
        qlabel_resize(label, 640, 480);
        qlabel_show(label);
        qapplication_exec(app);
    }
}
