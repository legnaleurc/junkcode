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

    public Cart() {
        this.items_ = new Hashtable< String, Integer >();
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
