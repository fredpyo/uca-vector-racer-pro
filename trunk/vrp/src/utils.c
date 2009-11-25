/*
 *
 * @description: Funciones generales utilitarias
 */
 
/**
 * Aproximar value a la siguiente potencia de increment
 */
int nearest_f(float value, int increment) {
	int x = increment;
	while (x * increment < value) x = x * increment;
	return x;
}

/**
 * Restringir a value incrementado en increment entre minimum y maximum
 */
float constraint_f(float value, float increment, float minimum, float maximum) {
    value += increment;
    if (value < minimum) value = minimum;
    if (value > maximum) value = maximum;
    return value;
}

/**
 * Incrementar value en increment y si supera maximun, hacer que desbore a minimum o vice versa
 */
float wrap_f(float value, float increment, float minimum, float maximum) {
    value += increment;
    if (value < minimum) value += maximum;
    if (value > maximum) value -= maximum;
    return value;
}
