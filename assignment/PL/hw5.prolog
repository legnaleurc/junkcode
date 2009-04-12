merge( [], L, L ).
merge( L, [], L ).
merge( [Ha|Ra], [Hb|Rb], [Ha|M] ) :- Ha >= Hb, merge( Ra, [Hb|Rb], M ).
merge( [Ha|Ra], [Hb|Rb], [Hb|M] ) :- Ha < Hb, merge( [Ha|Ra], Rb, M ).

flatten( [], [] ).
flatten( A, [A] ) :- atomic( A ).
flatten( [H|A], B ) :- flatten( H, Ht ), flatten( A, At ), append( Ht, At, B ).
