;; -*- lexical-binding: t; -*-
(require 'subr-x)

(defvar qb-mode-syntax-table
  (let ((table (make-syntax-table)))
    (modify-syntax-entry ?/ ". 124b" table)
    (modify-syntax-entry ?* ". 23" table)
    (modify-syntax-entry ?\n "> b" table)
    (modify-syntax-entry ?# "." table)
    (modify-syntax-entry ?' "\"" table)
    (modify-syntax-entry ?< "." table)
    (modify-syntax-entry ?> "." table)
    (modify-syntax-entry ?& "." table)
    (modify-syntax-entry ?% "." table)
    table)
  "Syntax table for `qb-mode'.")

(defvar qb-mode--types
  '("char" "void" "bool"
    "i4" "i8" "i16" "i32" "i64"
    "u4" "u8" "u16" "u32" "u64"
    "f4" "f8" "f16" "f32" "f64")
  "Built-in types in QB.")

(defvar qb-mode--keywords
  '("import" "extern" "while" "for" "goto" "typeof" "label"
    "type" "template" "pub" "static" "cast"
    "break" "case" "const" "continue" "default"
    "else" "enum" "return" "struct" "switch"
    "union" "while" "and_eq" "asm" "export" "false" "not" "not_eq"
    "nullptr" "operator" "or" "or_eq" "private" "protected" "public"
    "reinterpret_cast" "requires" "static_assert" "static_cast"
    "template" "this" "thread_local" "throw" "true" "typename"
    "using" "virtual" "xor" "xor_eq" "if" "else" "label" "while")
  "Keywords in QB.")

(defun qb--regexp-opt-symbols (words)
  (concat "\\_<" (regexp-opt words t) "\\_>"))

(defun qb-font-lock-keywords ()
  (let ((kw (qb--regexp-opt-symbols qb-mode--keywords))
        (types (qb--regexp-opt-symbols qb-mode--types)))
    `(
      ("^\\s-*#\\w+" . font-lock-preprocessor-face)
      ("\\<import\\>\\s-+\\(\"[^\"]+\"\\|<[^>]+>\\)" 1 font-lock-string-face)
      (,types . font-lock-type-face)
      (,kw . font-lock-keyword-face)
      ("\\b[0-9]+\\b" . font-lock-constant-face)
      ("^\\s-*\\([A-Za-z_][A-Za-z0-9_]*\\)\\s-*\\(::\\|->\\)" 1 font-lock-function-name-face)
      )))

(defun qb--previous-non-empty-line ()
  (save-excursion
    (forward-line -1)
    (while (and (not (bobp))
                (string-empty-p (string-trim-right (thing-at-point 'line t))))
      (forward-line -1))
    (if (bobp) nil (thing-at-point 'line t))))

(defun qb--indentation-of-previous-non-empty-line ()
  (save-excursion
    (forward-line -1)
    (while (and (not (bobp))
                (string-empty-p (string-trim-right (thing-at-point 'line t))))
      (forward-line -1))
    (current-indentation)))

(defun qb--desired-indentation ()
  (let* ((cur-line (string-trim-right (thing-at-point 'line t)))
         (prev-line (or (string-trim-right (qb--previous-non-empty-line)) ""))
         (indent-len 4)
         (prev-indent (or (qb--indentation-of-previous-non-empty-line) 0)))
    (cond
     ((string-match-p "^\\s-*if\\s-*(.*)\\s-*[^;{]+;\\s-*$" prev-line)
      prev-indent)
     ((and (not (string-empty-p prev-line))
           (string-suffix-p "{" (string-trim-right prev-line)))
      (+ prev-indent indent-len))
     ((string-prefix-p "}" (string-trim-left cur-line))
      (max (- prev-indent indent-len) 0))
     ((string-suffix-p ":" (string-trim-right prev-line))
      (if (string-suffix-p ":" (string-trim-right cur-line))
          prev-indent
        (+ prev-indent indent-len)))
     (t prev-indent))))

(defun qb-indent-line ()
  (interactive)
  (let ((desired (qb--desired-indentation))
        (offset (- (current-column) (current-indentation))))
    (indent-line-to desired)
    (when (> offset 0)
      (forward-char offset))))

(define-derived-mode qb-mode prog-mode "QB"
  :syntax-table qb-mode-syntax-table
  (setq-local font-lock-defaults '( (qb-font-lock-keywords) ))
  (setq-local indent-line-function 'qb-indent-line)
  (setq-local comment-start "// ")
  (setq-local comment-end "")
  (font-lock-add-keywords
   nil
   '(("\t" 0 '(:background "gray20" :foreground "orange") t)
     ("[ ]+$" 0 '(:background "gray20") t))))

(add-to-list 'auto-mode-alist '("\\.qb\\'" . qb-mode))

(provide 'qb-mode)
