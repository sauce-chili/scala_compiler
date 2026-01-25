package rtl;

public final class StdIn {

    private static final BufferedReader IN =
            new BufferedReader(new InputStreamReader(System.in));

    public StdIn() {}

    /** Read a full line. Returns null on EOF. */
    public String readLine() {
        try {
            return IN.readLine();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /** Read boolean from a line. Throws EOFException on EOF. */
    public boolean readBoolean() {
        String s = readLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));

        String lower = s.toLowerCase();
        return lower.equals("true")
                || lower.equals("t")
                || lower.equals("yes")
                || lower.equals("y");
    }

    /** Read a char from a line. Throws EOFException on EOF. */
    public char readChar() {
        String s = readLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        return s.charAt(0);
    }

    /** Read an int from a line. Throws EOFException on EOF. */
    public int readInt() {
        String s = readLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        return Integer.parseInt(s);
    }

    /** Read a double from a line. Throws EOFException on EOF. */
    public double readDouble() {
        String s = readLine();
        if (s == null)
            throw new RuntimeException(new EOFException("Console has reached end of input"));
        return Double.parseDouble(s);
    }
}
