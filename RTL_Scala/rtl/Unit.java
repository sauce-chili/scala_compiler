package rtl;

/**
 * Unit — singleton type. Используется для выражений, которые не возвращают полезного значения (аналог void).
 */
public final class Unit extends Any {
    public static final Unit INSTANCE = new Unit();

    public Unit() {}

    @Override
    public boolean equals(Object o) {
        return o instanceof Unit;
    }

    @Override
    public int hashCode() {
        return 0;
    }

    @Override
    public java.lang.String toString() {
        return "Unit";
    }

    @Override
    public String toScalaString() {
        return new String("Unit");
    }
}