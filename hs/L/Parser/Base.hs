module L.Parser.Base where

import Control.Monad
import Text.ParserCombinators.Parsec

import L.Core

l :: Parser L -> Parser L
l exp = do
    x <- exp
    eof
    return x

var :: Parser L -> Parser L
var = var' (noneOf "λ.() \n")

var' :: Parser Char -> Parser L -> Parser L
var' x _ = liftM Var (many1 x)

lam :: Parser L -> Parser L
lam exp = do
    char 'λ'
    x <- var exp
    char '.'
    y <- exp
    return (Lam x y)

app :: Parser L -> Parser L
app exp = liftM foldApp $ parens (exp `sepBy` many1 space)
  where
    parens = between (char '(') (char ')')

lang :: [Parser L -> Parser L] -> Parser L
lang (x:xs) = let y = foldl (<|>) (x y) (map ($y) xs) in y

base :: Parser L
base = lang [lam, var, app]

p :: String -> L
p = p' base

p' :: Parser L -> String -> L
p' exp = either (error . show) id . parse (l exp) ""
