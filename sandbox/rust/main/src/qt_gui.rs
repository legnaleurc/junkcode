extern crate libc;

use std;
use std::ffi::CString;
use self::libc::c_int;
use self::libc::c_char;
use self::libc::c_uint;


enum QtHandle {}


#[link(name="qt5rust")]
extern {
    fn qapplication_new(argc: c_int, argv: *mut *mut c_char) -> *mut QtHandle;
    fn qapplication_exec(this: *mut QtHandle) -> c_int;

    fn qlabel_new(parent: *mut QtHandle) -> *mut QtHandle;
    fn qlabel_resize(this: *mut QtHandle, width: c_int, height: c_int) -> ();
    fn qlabel_show(this: *mut QtHandle) -> ();
    fn qlabel_set_text(this: *mut QtHandle, utf8: *const c_char, length: c_uint) -> ();
}


pub struct QApplication {
    this: *mut QtHandle,
}


impl QApplication {

    pub fn new() -> QApplication {
        let args = std::env::args().map(|arg| CString::new(arg).unwrap() ).collect::<Vec<CString>>();
        let c_args = args.iter().map(|arg| arg.as_ptr()).collect::<Vec<*const c_char>>();

        unsafe {
            return QApplication {
                this: qapplication_new(c_args.len() as c_int, c_args.as_ptr() as *mut *mut c_char),
            };
        }
    }

    pub fn exec(&self) -> i32 {
        unsafe {
            return qapplication_exec(self.this);
        }
    }

}


pub struct QLabel {
    this: *mut QtHandle,
}


impl QLabel {

    pub fn new() -> QLabel {
        unsafe {
            return QLabel {
                this: qlabel_new(std::ptr::null_mut()),
            };
        }
    }

    pub fn resize(&self, width: i32, height: i32) -> () {
        unsafe {
            return qlabel_resize(self.this, width, height);
        }
    }

    pub fn show(&self) -> () {
        unsafe {
            return qlabel_show(self.this);
        }
    }

    pub fn set_text(&self, string: &str) -> () {
        let cs = CString::new(string).unwrap();
        unsafe {
            return qlabel_set_text(self.this, cs.as_ptr(), string.len() as u32);
        }
    }

}
