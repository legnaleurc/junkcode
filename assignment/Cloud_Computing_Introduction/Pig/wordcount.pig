-- read lines
lines = LOAD 'input.txt' AS (line:chararray);
words = FOREACH lines GENERATE FLATTEN( TOKENIZE(line) );
-- filter words length >= 3
words = FILTER words BY SIZE($0)>=3;
-- group same words
groups = GROUP words BY $0;
-- generate word count
index = FOREACH groups GENERATE COUNT($1), $0;
-- filter word count >= 2
result = FILTER index BY $0>=2;
-- store result
STORE result INTO 'output';
