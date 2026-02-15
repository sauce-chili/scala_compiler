package rtl;

class ArrayIterator extends rtl.Iterator {
    private final Array array;

    private int index = 0;

    ArrayIterator(rtl.Array array) {
        this.array = array;
    }

    @Override
    public rtl.Boolean hasNext() {
        return rtl.Boolean.valueOf(index < array.length().intValue());
    }

    @Override
    public rtl.Any next() {
        if (index >= array.length().intValue()) throw new java.util.NoSuchElementException();
        return array.apply(new rtl.Int(index++));
    }
}
