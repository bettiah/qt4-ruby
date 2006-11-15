(defconstant +h+
  "#ifndef ~a_H~@
   #define ~a_H~@
   ~@
   #include <~a>~@
   ~@
   class ~a : public ~a~@
   {~@
   ~aQ_OBJECT~@
   ~@
   public:~@
   ~a~a(QObject *parent = 0);~@
   ~@
   public slots:~@
   ~a// note: on name collisions use an underscore (this is handled automatically in \"cqt.cpp\")~@
   };~@
   ~@
   #endif // ~a_H~%")

(defconstant +cpp+
  "#include \"~a\.h\"~@
   ~@
   ~a::~a(QObject *parent)~@
   ~a: ~a((QWidget*)parent)~@
   {~@
   }~%")

(defun write-file (type name &rest args)
  (with-open-file (s (format nil "base/~a.~a" name (string-downcase type))
		     :direction :output :if-exists nil)
    (if s
	(apply #'format s args)
	(error "Subclass file already exists!"))))

(defun underscore (name)
  (with-output-to-string (s)
    (write-char (char-downcase (char name 0)) s)
    (map nil (lambda (ch) (if (upper-case-p ch)
			      (format s "_~c" (char-downcase ch))
			      (write-char ch s)))
	 (subseq name 1))))

(defun run (class)
  (let* ((sub-class (subseq class 1))
	 (upper (string-upcase   sub-class))
	 (lower (string-downcase sub-class))
	 (lower_ (underscore sub-class))
	 (spaces "    "))
    (write-file 'h   lower_ +h+   upper upper class sub-class class spaces spaces sub-class spaces upper)
    (write-file 'cpp lower_ +cpp+ lower_ sub-class sub-class spaces class)))

; example: (run "QMenu")
