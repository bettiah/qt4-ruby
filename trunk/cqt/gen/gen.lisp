;;; (c) Copyright 2006 Paul Ruetz (BSD License)
;;; feedback: paul AT myteam-software DOT com

(defparameter *class-files* nil)

(defun gen (file frm fn &key count (char #\Tab))
  (with-open-file (s (format nil "include/~a" file) :direction :output :if-exists :supersede)
    (let ((n 0))
      (dolist (el *class-files*)
	(let ((name (subseq el (1+ (position #\/ el)))))
	  (if count
	      (progn
		(incf n)
		(let ((name (funcall fn name)))
		  (when name
		    (format s frm char n name))))
	      (format s frm char (funcall fn name))))))))

(defun object-name (name)
  (substitute #\- #\_ name))

(defun new-name (name)
  (with-output-to-string (s)
    (write-char (char-upcase (char name 0)) s)
    (dotimes (n (1- (length name)))
      (let ((ch (char name (1+ n))))
	(write-char (if (char= #\_ ch)
			(char-upcase (char name (1+ (incf n))))
			ch)
		    s)))))

(defun include-name (name)
  (format nil "<~a.h>" name))

(defun read-class-files ()
  (let (names)
    (with-open-file (s "../cqt.pro" :direction :input)
      (do ((line (read-line s nil :eof) (read-line s nil :eof)))
	  ((eql :eof line))
	(when (and (find #\/ line)
		   (search ".h" line))
	  (let ((name (string-trim (format nil " \\~a" #\Tab)  line)))
	    (push (subseq name 0 (- (length name) 2))
		  names)))))
    (nreverse names)))

(defun make-gen-pro ()
  (with-open-file (s "gen.pro" :direction :output :if-exists :supersede)
    (format s "TEMPLATE = app~@
               INCLUDEPATH = ../base ../extensions~@
               OBJECTS_DIR = tmp~@
               MOC_DIR = tmp~@
               TARGET = gen~%~@
               HEADERS = \\~%")
    (format s "~{    ../~a~^.h \\~%~}" *class-files*)
    (format s ".h~%~%SOURCES = gen.cpp \\~%")
    (format s "~{    ../~a~^.cpp \\~%~}.cpp" *class-files*)
    (format s "~%~%DESTDIR = ./~@
               TARGET = gen")))

(defun run ()
  (setf *class-files* (read-class-files))
  (make-gen-pro)
  (mapc #'(lambda (a) (apply #'gen a))
	'(("objects_names" "~c\"~a\",~%" object-name)
	  ("objects_new" "~c\case ~3d: return new ~a(parent);~%" new-name :count t)
	  ("objects_include" "~cinclude ~a~%" include-name :char #\#))))

(run)

(quit)
