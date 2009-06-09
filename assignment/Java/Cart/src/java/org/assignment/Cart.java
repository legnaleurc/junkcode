/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.assignment;

import java.util.Hashtable;
import java.util.Map;

/**
 *
 * @author legnaleurc
 */
public class Cart {

    private Hashtable< String, Integer > items_;
    static final private String UpperCases_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static final private String LowerCases_ = UpperCases_.toLowerCase();
    static final private Hashtable< String, Integer > Items_ = new Hashtable();

    static {
        Items_.put( "Amarok", 2 );
        Items_.put( "Boson", 3 );
        Items_.put( "CMake", 5 );
        Items_.put( "Dolphin", 7 );
        Items_.put( "Firefox", 11 );
        Items_.put( "GDB", 13 );
        Items_.put( "Htop", 17 );
        Items_.put( "Iceweasel", 19 );
        Items_.put( "JuK", 23 );
        Items_.put( "Kate", 29 );
        Items_.put( "Lyx", 31 );
        Items_.put( "Mozilla", 37 );
        Items_.put( "NetScape", 41 );
        Items_.put( "Okular", 43 );
        Items_.put( "Plasma", 47 );
        Items_.put( "Qwit", 53 );
        Items_.put( "Regex", 59 );
        Items_.put( "Solid", 61 );
        Items_.put( "Tar", 67 );
        Items_.put( "Umbrello", 71 );
        Items_.put( "Vanila", 73 );
        Items_.put( "Webkit", 79 );
        Items_.put( "Xorg", 83 );
        Items_.put( "Yacc", 89 );
        Items_.put( "ZFS", 97 );
    }

    public Cart() {
        this.items_ = new Hashtable< String, Integer >();
    }

    static public Hashtable< String, Integer > loadItems() {
        return Items_;
    }

    static public Integer randomValue( Integer from, Integer to ) {
        return from + ( int )( Math.floor( Math.random() * ( to - from ) ) );
    }
    static private String randomString_( Integer length ) {
        StringBuilder sin = new StringBuilder();
        for( ; length > 0; --length ) {
            sin.append( LowerCases_.charAt( randomValue( 0, LowerCases_.length() ) ) );
        }
        return sin.toString();
    }
    static public String randomName( Integer length ) {
        StringBuilder sin = new StringBuilder();
        sin.append( UpperCases_.charAt( randomValue( 0, UpperCases_.length() ) ) );
        sin.append( randomString_( length - 1 ) );
        return sin.toString();
    }
    static public String randomBrief( Integer sections ) {
        StringBuilder sin = new StringBuilder();
        sin.append( randomName( randomValue( 2, 10 ) ) );

        for( int section = 0; section < sections; ++section ) {
            int words = randomValue( 2, 10 );
            for( int word = 0; word < words; ++word ) {
                sin.append( ' ' );
                sin.append( randomString_( randomValue( 2, 10 ) ) );
            }
            sin.append( ',' );
        }

        sin.replace(sin.length()-1, sin.length(), ".");
        return sin.toString();
    }

    public Cart addItem( String key, Integer value ) {
        this.items_.put(key, value);
        return this;
    }
    public Integer getItem( String key ) {
        return this.items_.get( key );
    }
    public Integer getTotalValue() {
        Integer sum = 0;
        for( Map.Entry< String, Integer > e : this.items_.entrySet() ) {
            sum += e.getValue() * Items_.get( e.getKey() );
        }
        return sum;
    }

}
