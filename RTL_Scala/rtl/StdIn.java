package rtl;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.EOFException;

public final class StdIn extends Any {

    private static final BufferedReader IN =
            new BufferedReader(new InputStreamReader(System.in));

    public StdIn() {}

    /** Read a full line. Returns null on EOF. */
    public java.lang.String readLine() {
        try {
            return IN.readLine();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

//     public rtl.String readLine() {
//         java.lang.String s = readLine();
//         if (s == null) throw new RuntimeException(new EOFException("Console has reached end of input"));
//         return new String(s);
//     }

    /** Read boolean from a line. Throws EOFException on EOF. */
    public boolean readBoolean() {
        java.lang.String s = readLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));

        java.lang.String lower = s.toLowerCase();
        return lower.equals("true")
                || lower.equals("t")
                || lower.equals("yes")
                || lower.equals("y");
    }

//     public Boolean readBoolean() {
//         return Boolean.valueOf(readBoolean());
//     }

    /** Read a char from a line. Throws EOFException on EOF. */
    public char readChar() {
        java.lang.String s = readLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        return s.charAt(0);
    }

//     public Char readChar() {
//         return new rtl.Char(readChar());
//     }

    /** Read an int from a line. Throws EOFException on EOF. */
    public int readInt() {
        java.lang.String s = readLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        return Integer.parseInt(s);
    }

//     public Int readInt() {
//         return new rtl.Int(readInt());
//     }

    /** Read a double from a line. Throws EOFException on EOF. */
    public double readDouble() {
        java.lang.String s = readLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        return java.lang.Double.parseDouble(s);
    }

//     public Double readDouble() {
//         return new rtl.Double(readDouble());
//     }

    @Override
    public boolean equals(Object o) {
        return o instanceof StdIn;
    }

    @Override
    public int hashCode() {
        return 0;
    }
}
