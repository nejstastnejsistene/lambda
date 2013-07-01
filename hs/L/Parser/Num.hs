module L.Parser.Num where

import Text.ParserCombinators.Parsec

import L.Church.Natural
import L.Core
import L.Parser.Base hiding (p)

numExp :: Parser L
numExp = lang [lam, num, var, app]

num :: Parser L -> Parser L
num _ = many1 digit >>= return . toChurch . read

p = p' numExp
