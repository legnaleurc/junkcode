/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.assignment;

import java.util.Hashtable;

/**
 *
 * @author legnaleurc
 */
public class Cart {

    private Hashtable< String, Integer > items_;
    static final private String UpperCases_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static final private String LowerCases_ = UpperCases_.toLowerCase();

    public Cart() {
        this.items_ = new Hashtable< String, Integer >();
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
        for( Integer i : this.items_.values() ) {
            sum += i;
        }
        return sum;
    }

}
