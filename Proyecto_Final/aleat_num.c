/**
 * @brief Fichero con la función aleat_num
 * 
 * En este fichero definimos la función aleat_num,
 * ya que es utilizada en más de un fichero, y
 * para así evitar reutilización del código
 * 
 * @file aleat_num.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 6-4-2018
 */
 
#include <stdlib.h>


/**
 * @brief Genera un número aleatorio
 *
 * Esta funcion genera un número aleatorio entre dos
 * especificados
 * 
 * @param inf Menor número posible
 * @param sup Mayor número posible
 * @return Número generado
 */
int aleat_num(int inf, int sup) {
  
  int dif;
  
  if (inf > sup) return inf - 1;
  
  dif = sup - inf;
  
  return inf + (int)((dif + 1) * (rand() / (RAND_MAX + 1.0)));

}