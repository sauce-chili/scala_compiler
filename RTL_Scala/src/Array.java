package rtl;

import java.util.Arrays;

public final class Array extends Any {

    private final Any[] elements;

    /** Создание массива фиксированной длины, заполненного null. */
    public Array(Int length) {
        int n = length.intValue();
        if (n < 0) throw new IllegalArgumentException("Negative array size");
        this.elements = new Any[n];
    }

    /** Создание массива из Java-массива RTL-элементов. */
    public Array(Any[] elems) {
        this.elements = elems == null ? new Any[0] : elems.clone();
    }

    /** Длина массива (аналог array.length). */
    public Int length() {
        return new Int(elements.length);
    }

    /** Доступ к элементу (аналог array(i)). */
    public Any apply(Int index) {
        int i = index.intValue();
        return elements[i];
    }

    public Unit update(Int index, Any value) {
        int i = index.intValue();
        elements[i] = value;
        return Unit.INSTANCE;
    }

    @Override
    public String toScalaString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        for (int i = 0; i < elements.length; i++) {
            Any e = elements[i];
            sb.append(e == null ? "null" : e.toString());
            if (i + 1 < elements.length) sb.append(", ");
        }
        sb.append("]");
        return new String(sb.toString());
    }

    @Override
    public java.lang.String toString() {
        return toScalaString().nativeValue();
    }

    @Override
    public boolean equals(Object other) {
        if (!(other instanceof Array)) return false;
        Array o = (Array) other;
        return Arrays.equals(this.elements, o.elements);
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(elements);


    public Boolean notEquals(Any other) {
        return Boolean.valueOf(!this.equals(other));
    }

    public Boolean is(Any other) {
        return Boolean.valueOf(this == other);
    }

    public Boolean isNot(Any other) {
        return Boolean.valueOf(this != other);
    }

    public Boolean isInstanceOf(Object cls) {
        return Boolean.valueOf(cls.isInstance(this));
    }
}
