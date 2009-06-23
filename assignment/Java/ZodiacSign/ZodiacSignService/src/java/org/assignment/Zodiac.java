/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.assignment;

import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;

/**
 *
 * @author legnaleurc
 */
@WebService()
public class Zodiac {

    /**
     * Web service operation
     */
    @WebMethod(operationName = "judge")
    public String judge(@WebParam(name = "month")
    int month, @WebParam(name = "date")
    int date) {
        switch( month ) {
            case 1:
                return ( date < 20 ? "Capricorn" : "Aquarius" );
            case 2:
                return ( date < 18 ? "Aquarius" : "Pisces" );
            case 3:
                return ( date < 20 ? "Pisces" : "Aries" );
            case 4:
                return ( date < 19 ? "Aries" : "Taurus" );
            case 5:
                return ( date < 20 ? "Taurus" : "Gemini" );
            case 6:
                return ( date < 21 ? "Gemini" : "Cancer" );
            case 7:
                return ( date < 22 ? "Cancer" : "Leo" );
            case 8:
                return ( date < 23 ? "Leo" : "Virgo" );
            case 9:
                return ( date < 22 ? "Virgo" : "Libra" );
            case 10:
                return ( date < 23 ? "Libra" : "Scorpio" );
            case 11:
                return ( date < 22 ? "Scorpio" : "Sagittarius" );
            case 12:
                return ( date < 21 ? "Sagittarius" : "Capricorn" );
            default:
                return null;
        }
    }
}
