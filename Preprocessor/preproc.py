#!/usr/bin/python3

import os
import sys

import numpy as numpy
import cv2

OUTPUT_FILE = 'blocks_array.txt'

GRID_ROWS = 24
GRID_COLS = 24

def createBlocks(img):
    blocksArray = []

    # Dimensiones de la imagen
    img_height = img.shape[0]
    img_width = img.shape[1]

    # Dimensiones (px) de cada bloque
    grid_height = img_height // GRID_ROWS
    grid_width = img_width // GRID_COLS

    # Contadores de fila
    rowStart = 0
    rowEnd = grid_height

    # Contadores de columna
    colStart = 0
    colEnd = grid_width

    for i in range(GRID_ROWS):
        for j in range(GRID_COLS):
            # Obtiene bloque / submatriz de la imagen
            block = img[rowStart:rowEnd, colStart:colEnd]
            blocksArray.append(block)

            # Incrementa contadores de columna
            colStart += grid_width
            if j == GRID_COLS - 1: # Si es la ultima columna
                colEnd = img_width - 1 # Usa el ancho de la imagen como pixel final
            else:
                colEnd += grid_width

        # Resetea contadores de columna
        colStart = 0
        colEnd = grid_width

        # Incrementa contadores de fila
        rowStart += grid_height
        if i == GRID_ROWS - 1: # Si es la ultima fila
            rowEnd = img_height - 1 # Usa la altura de la imagen como pixel final
        else:
            rowEnd += grid_height


    return blocksArray

def listToFile(_list):
    # Si el archivo ya existe lo borra
    if os.path.exists(OUTPUT_FILE):
        os.remove(OUTPUT_FILE)
    
    with open(OUTPUT_FILE, 'w') as f:
        for item in _list:
            f.write("%s\n" % item)

        f.write(" ") # Espacio en blanco para delimitar final

def main(argv):
    imagen = cv2.imread(argv[0], 0) # Path en primer argumento, Flag 0 para grayscale
    imagen = cv2.bitwise_not(imagen) # Invierte los colores de la escala de gris

    blocks = createBlocks(imagen)
    averages = []

    for block in blocks:
        average = int(round(block.mean()))
        averages.append(average)

        # Mostrar una ventana por cada bloque extraido
        #cv2.imshow("Block", block)
        #cv2.waitKey(0)
        #cv2.destroyAllWindows()

    # Escribe la lista a memoria
    listToFile(averages)


if __name__ == "__main__":
    main(sys.argv[1:])