@echo off
copy /y proposal.html temp.html
html_include_files temp.html proposal.html
proposal.html
 