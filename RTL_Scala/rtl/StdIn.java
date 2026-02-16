package rtl;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.EOFException;

public final class StdIn extends Any {

    private static final BufferedReader IN =
            new BufferedReader(new InputStreamReader(System.in));

    public StdIn() {}

    public java.lang.String readConsoleLine() {
        try {
            return IN.readLine();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /** Read a full line. Returns null on EOF. */
    public rtl.String readLine() {
        java.lang.String s = readConsoleLine();
        if (s == null) throw new RuntimeException(new EOFException("Console has reached end of input"));
        return new rtl.String(s);
    }

    /** Read boolean from a line. Throws EOFException on EOF. */
    public rtl.Boolean readBoolean() {
        java.lang.String s = readConsoleLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        java.lang.String lower = s.toLowerCase();
        boolean readVal = lower.equals("true")
                || lower.equals("t")
                || lower.equals("yes")
                || lower.equals("y");
        return Boolean.valueOf(readVal);
    }

    /** Read a char from a line. Throws EOFException on EOF. */
    public rtl.Char readChar() {
        java.lang.String s = readConsoleLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        char readVal = s.charAt(0);
        return new rtl.Char(readVal);
    }

    /** Read an int from a line. Throws EOFException on EOF. */
    public rtl.Int readInt() {
        java.lang.String s = readConsoleLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        int readVal = Integer.parseInt(s);
        return new rtl.Int(readVal);
    }

    /** Read a double from a line. Throws EOFException on EOF. */
    public rtl.Double readDouble() {
        java.lang.String s = readConsoleLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        float readVal = java.lang.Float.parseFloat(s);
        return new rtl.Double(readVal);
    }

    @Override
    public boolean equals(Object o) {
        return o instanceof StdIn;
    }

    @Override
    public int hashCode() {
        return 0;
    }
}
