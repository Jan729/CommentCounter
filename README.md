# CommentCounter

Accepts file names, then scans those files and reports the total number of:
1) Lines
2) Comment lines (lines with both code and a comment are included)
3) Single comment lines
4) Comment lines within block comments
5) Block line comments
6) TODOs

Assumptions:
1) User will enter the name of the file they wish to check, one file per line
		(subsequent file names will be ignored)
2) Output will appear on the console
3) Files won't contain nested block comments
