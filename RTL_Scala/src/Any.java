package rtl;

/**
 * Базовый тип Any (условный — похож на scala.Any).
 *
 * Subclasses должны по-прежнему переопределять equals(Object), hashCode() и toString().
 */
public abstract class Any {
    /**
     * Возвращает строковое представление как rtl.String.
     * По умолчанию использует java.lang.Object#toString().
     */
    public String toScalaString() {
        return String.valueOf(this.toString());
    }

    /**
     * Проверка типа (аналог isInstanceOf[T]).
     */
    public Boolean isInstanceOf(Object cls) {
        return Boolean.valueOf(cls.isInstance(this));
    }
}