Functions in here are installed as Yisp functions. They generally start with `fn_` for a general function, `tf_` for a function that supports tail-call optimization, and `sp_` for macros/special forms.

Most of these are extraneous, and if you don't use them, they can be deleted if you're short on flash/RAM, but some -- such as `tf_progn` (used to perform the "implicit `progn`s" that some macros do) -- are called by others, and must remain.
