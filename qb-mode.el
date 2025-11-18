;; -*- lexical-binding: t; -*-
(require 'subr-x)

(defvar qb-mode-syntax-table
  (let ((table (make-syntax-table)))
    ;; C/C++ style comments
	(modify-syntax-entry ?/ ". 124b" table)
	(modify-syntax-entry ?* ". 23" table)
	(modify-syntax-entry ?\n "> b" table)
    ;; Preprocessor stuff?
    (modify-syntax-entry ?# "." table)
    ;; Chars are the same as strings
    (modify-syntax-entry ?' "\"" table)
    ;; Treat <> as punctuation (needed to highlight C++ keywords
    ;; properly in template syntax)
    (modify-syntax-entry ?< "." table)
    (modify-syntax-entry ?> "." table)

    (modify-syntax-entry ?& "." table)
    (modify-syntax-entry ?% "." table)
    table))

(defun qb-types ()
  '("char" "void" "bool"
    "i4" "i8" "i16" "i32" "i64"
    "u4" "u8" "u16" "u32" "u64"
    "f4" "f8" "f16" "f32" "f64"))

(defun qb-keywords ()
  '("break" "case" "const" "continue" "default" 
    "else" "enum" "extern" "for" "goto" "if" "label"
    "return" "" "static" "struct" "switch"
    "union"  "type" "while"
    "and_eq" "asm" "cast" "export" "false" "not" "not_eq"
    "nullptr" "operator" "or" "or_eq" "private" "protected" "public" "reflexpr"
    "reinterpret_cast" "requires" "static_assert" "static_cast" "synchronized"
    "template" "this" "thread_local" "throw" "true" "typename"
    "using" "virtual" "wchar_t" "xor" "xor_eq"))

(defun qb-font-lock-keywords ()
  (list
   `("*[#a-zA-Z0-9_]+" . font-lock-preprocessor-face)
   `("import \\(\\(<\\|\"\\).*\\(>\\|\"\\)\\)" . (1 font-lock-string-face))
   `(,(regexp-opt (simpc-keywords) 'symbols) . font-lock-keyword-face)
   `(,(regexp-opt (simpc-types) 'symbols) . font-lock-type-face)))

(defun qb--previous-non-empty-line ()
  (save-excursion
    (forward-line -1)
    (while (and (not (bobp))
                (string-empty-p
                 (string-trim-right
                  (thing-at-point 'line t))))
      (forward-line -1))
    (thing-at-point 'line t)))

(defun qb--indentation-of-previous-non-empty-line ()
  (save-excursion
    (forward-line -1)
    (while (and (not (bobp))
                (string-empty-p
                 (string-trim-right
                  (thing-at-point 'line t))))
      (forward-line -1))
    (current-indentation)))

(defun qb--desired-indentation ()
  (let* ((cur-line (string-trim-right (thing-at-point 'line t)))
         (prev-line (string-trim-right (simpc--previous-non-empty-line)))
         (indent-len 4)
         (prev-indent (simpc--indentation-of-previous-non-empty-line)))
    (cond
     ;; case: if (...) <statement>;   → do not indent next line
     ((string-match-p "^\\s-*if\\s-*[^]*)\\s-*[^;]+;\\s-*$" prev-line)
      prev-indent)

     ;; existing rules …
     ((string-match-p "^\\s-*switch\\s-*(.+)" prev-line)
      prev-indent)
     ((and (string-suffix-p "{" prev-line)
           (string-prefix-p "}" (string-trim-left cur-line)))
      prev-indent)
     ((string-suffix-p "{" prev-line)
      (+ prev-indent indent-len))
     ((string-prefix-p "}" (string-trim-left cur-line))
      (max (- prev-indent indent-len) 0))
     ((string-suffix-p ":" prev-line)
      (if (string-suffix-p ":" cur-line)
          prev-indent
        (+ prev-indent indent-len)))
     ((string-suffix-p ":" cur-line)
      (max (- prev-indent indent-len) 0))
     ((string-match-p "^\\s-*if\\s-*" prev-line)
      (+ prev-indent indent-len))
     ((string-match-p "^\\s-*else" prev-line)
      prev-indent)
     (t prev-indent))))

;;; TODO: customizable indentation (amount of spaces, tabs, etc)
(defun qb-indent-line ()
  (interactive)
  (when (not (bobp))
    (let* ((desired-indentation
            (simpc--desired-indentation))
           (n (max (- (current-column) (current-indentation)) 0)))
      (indent-line-to desired-indentation)
      (forward-char n))))

(define-derived-mode qb-mode prog-mode "Simple C"
  "Simple major mode for editing C files."
  :syntax-table qb-mode-syntax-table
  (setq-local font-lock-defaults '(qb-font-lock-keywords))
  (setq-local indent-line-function 'qb-indent-line)
  (setq-local comment-start "// "))

;; Highlight only special-case whitespace in qb-mode
(defun qb--show-whitespace ()
  "Highlight tabs and trailing spaces in `qb-mode`."
  (font-lock-add-keywords
   nil
   '(("\t"     0 '(:background "gray20" :foreground "orange") t) ; tabs
     ("[ ]+$"  0 '(:background ".") t))))                       ; trailing spaces

(add-hook 'qb-mode-hook #'qb--show-whitespace)


(provide 'qb-mode)
