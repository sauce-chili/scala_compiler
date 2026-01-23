package rtl;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.EOFException;

/**
 * Утилиты ввода-вывода.
 *
 * Методы используют типы из rtl (rtl.String, rtl.Int, rtl.Float, rtl.Char, rtl.Boolean).
 * Методы ввода кидают RuntimeException(EOFException) при EOF (кроме readLine — она возвращает null).
 */
public final class SbtIn extends Any {
    private static final BufferedReader IN = new BufferedReader(new InputStreamReader(System.in));
    private static final PrintStream OUT = System.out;
    private static final PrintStream ERR = System.err;

    private SbtIn() { /* static only */ }

    /* -------------------- Input -------------------- */

    /**
     * Читает строку (без символа переноса). Возвращает null при EOF.
     */
    public static rtl.String readLine() {
        try {
            String s = IN.readLine();
            return (s == null) ? null : new rtl.String(s);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Печатает prompt и читает строку. Возвращает null при EOF.
     */
    public static rtl.String readLine(rtl.String prompt) {
        if (prompt != null) {
            OUT.print(prompt.nativeValue());
            OUT.flush();
        }
        return readLine();
    }

    /**
     * Читает строку и парсит в Int.
     * Бросает RuntimeException(EOFException) при EOF.
     * Бросает NumberFormatException при неправильном числе.
     */
    public static rtl.Int readInt() {
        rtl.String s = readLineOrThrow();
        return s.toInt();
    }

    /**
     * Читает строку и парсит в Float.
     * Бросает RuntimeException(EOFException) при EOF.
     * Бросает NumberFormatException при неправильном числе.
     */
    public static rtl.Float readFloat() {
        rtl.String s = readLineOrThrow();
        return s.toFloat();
    }

    /**
     * Читает строку и возвращает первый символ как rtl.Char.
     * Если строка пуста — IllegalArgumentException.
     * При EOF бросает RuntimeException(EOFException).
     */
    public static rtl.Char readChar() {
        rtl.String s = readLineOrThrow();
        return s.toChar();
    }

    /**
     * Читает строку и распознаёт boolean (true/false, case-insensitive).
     * При EOF бросает RuntimeException(EOFException).
     */
    public static rtl.Boolean readBoolean() {
        rtl.String s = readLineOrThrow();
        // Используем стандартный Java-парсер для boolean (только "true" => true, иначе false).
        java.lang.String nativeS = s.nativeValue().trim();
        return rtl.Boolean.valueOf(java.lang.Boolean.parseBoolean(nativeS));
    }

    private static rtl.String readLineOrThrow() {
        rtl.String s = readLine();
        if (s == null) {
            throw new RuntimeException(new EOFException("EOF while reading from console"));
        }
        return s;
    }

    /* -------------------- Output -------------------- */

    public static rtl.Unit print(rtl.String s) {
        OUT.print((s == null) ? "null" : s.nativeValue());
        return rtl.Unit.INSTANCE;
    }

    public static rtl.Unit println() {
        OUT.println();
        return rtl.Unit.INSTANCE;
    }

    public static rtl.Unit println(rtl.String s) {
        OUT.println((s == null) ? "null" : s.nativeValue());
        return rtl.Unit.INSTANCE;
    }

    public static rtl.Unit println(rtl.Int i) {
        OUT.println((i == null) ? "null" : i.toString());
        return rtl.Unit.INSTANCE;
    }

    public static rtl.Unit println(rtl.Float f) {
        OUT.println((f == null) ? "null" : f.toString());
        return rtl.Unit.INSTANCE;
    }

    public static rtl.Unit println(rtl.Char c) {
        OUT.println((c == null) ? "null" : c.toString());
        return rtl.Unit.INSTANCE;
    }

    public static rtl.Unit println(rtl.Boolean b) {
        OUT.println((b == null) ? "null" : b.toString());
        return rtl.Unit.INSTANCE;
    }

    /**
     * Универсальный println для любого rtl.Any. Использует toString() конкретного объекта.
     */
    public static rtl.Unit println(Any any) {
        OUT.println((any == null) ? "null" : any.toString());
        return rtl.Unit.INSTANCE;
    }

    /**
     * Печать в stderr.
     */
    public static rtl.Unit errPrintln(Any any) {
        ERR.println((any == null) ? "null" : any.toString());
        return rtl.Unit.INSTANCE;
    }

    /**
     * Форматированный вывод.
     * Пример: SbtIn.printf(new rtl.String("x=%d y=%s\n"), new rtl.Int(10), new rtl.String("hi"));
     * Форматирует через java.lang.String.format; аргументы приводятся через their toString().
     */
    public static rtl.Unit printf(rtl.String format, Any... args) {
        java.lang.String fmt = (format == null) ? "null" : format.nativeValue();
        Object[] nativeArgs = new Object[args == null ? 0 : args.length];
        if (args != null) {
            for (int i = 0; i < args.length; i++) {
                Any a = args[i];
                nativeArgs[i] = (a == null) ? "null" : a.toString();
            }
        }
        OUT.print(java.lang.String.format(fmt, nativeArgs));
        return rtl.Unit.INSTANCE;
    }
}
