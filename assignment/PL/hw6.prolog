/*
Generate a list [1, 2, ..., N] to L.
Permute L to Board.
Check Board if it is safe.
*/
queens( N, Board ) :- !, range( N, L ), permutation( L, Board ), safe( Board ).

% Generate a increasing list.
range( N, L ) :- decrease( N, R ), reverse( R, L ).
decrease( 0, [] ) :- !.
decrease( N, [H | L] ) :- N = H, Next is N - 1, decrease( Next, L ).

% Check if all queens are safe.
safe( [_] ) :- !.
safe( [Queen | Others] ) :- !, nohit( Queen, 1, Others ), safe( Others ).
% Check if queens could not attack each other.
nohit( _, _, [] ) :- !.
nohit( Column, Row_D, [Queen | Others] ) :- !, nohit( Column, Row_D, Queen ), Row_D_Next is Row_D + 1, nohit( Column, Row_D_Next, Others ).
nohit( Column, Row_D, Target ) :- !, Diag1 is Column + Row_D, Diag1 =\= Target, Diag2 is Column - Row_D, Diag2 =\= Target.
