#!/bin/sh
echo "index.html"
./webclient http://www.fit.vutbr.cz && echo "OK"
echo "2nd - image (fit_logo_cz.gif)"
./webclient http://www.fit.vutbr.cz:80/common/img/fit_logo_cz.gif && echo "OK"
echo "3nd - text file with space"
./webclient http://www.fit.vutbr.cz/study/courses/IPK/public/some\ text.txt && echo "OK"
echo "4nd - redirection"
./webclient http://www.fit.vutbr.cz/study/courses/IPK/public/test/redir.php && echo "OK"
