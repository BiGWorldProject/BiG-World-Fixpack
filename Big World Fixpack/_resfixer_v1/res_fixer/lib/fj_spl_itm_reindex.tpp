PATCH_IF !(~%SOURCE_FILE%~ STRING_MATCHES_REGEXP ~^.+\.spl~) BEGIN
  hs = 0x28
END ELSE PATCH_IF !(~%SOURCE_FILE%~ STRING_MATCHES_REGEXP ~^.+\.itm~) BEGIN
  hs = 0x38
END
READ_LONG 0x64 hf //Extended header offset
READ_SHORT 0x68 hc //Extended header count
READ_LONG 0x6a fb //Feature block table offset
READ_SHORT 0x70 cn //Casting feature block count
PATCH_IF ((hf > fb) AND (hc > 1)) BEGIN
  READ_ASCII hf ~eh~ ELSE ~fail~ (hs * hc)
  PATCH_IF (~%eh%~ STRING_EQUAL ~fail~) BEGIN
    WHILE ((~%eh%~ STRING_EQUAL ~fail~) AND (hc > 0)) BEGIN
      READ_ASCII hf ~eh~ ELSE ~fail~ (hs * hc)
      hc -= 1
    END
  END
  DELETE_BYTES hf (hs * hc)
  hf = 0x72
  WRITE_LONG 0x64 hf
  WRITE_SHORT 0x68 hc
  fb = (0x72 + (hs * hc))
  WRITE_LONG 0x6a fb
  PATCH_IF !(~%eh%~ STRING_EQUAL ~fail~) BEGIN
    INSERT_BYTES hf (hs * hc)
    WRITE_ASCIIE hf ~%eh%~
  END
END ELSE PATCH_IF ((hf != 0x72) AND (hc = 0)) BEGIN
  hf = 0x72
  WRITE_LONG 0x64 hf
END
FOR (i1 = 0; i1 < (hs * hc); i1 += hs) BEGIN
  WRITE_SHORT (hf + i1 + 0x20) cn
  READ_SHORT (hf + i1 + 0x1e) cx
  cn += cx
END
PATCH_IF (SOURCE_SIZE > (0x72 + (hs * hc) + (0x30 * cn))) BEGIN
  DELETE_BYTES (0x72 + (hs * hc) + (0x30 * cn)) (SOURCE_SIZE - (0x72 + (hs * hc) + (0x30 * cn)))
END
