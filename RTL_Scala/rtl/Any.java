package rtl;

/**
 * Базовый тип Any (условный — похож на scala.Any).
 *
 * Subclasses должны по-прежнему переопределять equals(Object), hashCode() и toString().
 */
public abstract class Any extends Object {
    /**
     * Возвращает строковое представление как rtl.String.
     * По умолчанию использует java.lang.Object#toString().
     */
    public String toScalaString() {
        return new String(this.toString());
    }

    /**
     * Проверка типа (аналог isInstanceOf[T]).
     */
    public Boolean isInstanceOf(Object cls) {
        return new Boolean(cls.getClass().isInstance(this));
    }
}