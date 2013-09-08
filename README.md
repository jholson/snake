snake
=====

Snake... next-gen

Developer set-up
=====

Style
=====
(robert): I am mostly following the rules from here:
  http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

Le Pro tips
=====
Tells you where g++ is looking for header files by asking gcc what preprocessor it will use
and then asking that preprocessor for where it will search for includes
  `gcc -print-prog-name=cc1plus` -v

