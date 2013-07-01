module L.Church.Natural
    ( plus
    , mult
    , exp'
    , toChurch
    , unChurch
    , isNum
    , showChurch
    , showChurch'
    ) where

import L.Core
import L.Parser.Base

nan = error "not a number"

instance Enum L where
    toEnum = toChurch
    fromEnum = maybe nan id . unChurch

instance Num L where
    x + y = applyN [plus, x, y]
    x * y = applyN [mult, x, y]
    abs x = maybe nan (const x) (unChurch x)
    signum = maybe nan (const 1) . unChurch
    fromInteger = toChurch . fromInteger

zero = p "λa.λb.b"
one  = p "λa.λb.(a b)"

plus = p "λa.λb.λc.λd.((a c) (b c d))"
mult = p "λa.λb.λc.(a (b c))"
exp' = p "λa.λb.(b a)"

toChurch :: Int -> L
toChurch 0 = zero
toChurch n = applyN [plus, one, toChurch $ n - 1]

unChurch :: L -> Maybe Int
unChurch (Lam (Var p) (Lam (Var q) r))
  | p /= q    = unChurch' p q r
  | otherwise = Nothing
unChurch _    = Nothing

unChurch' p q (Var r)
  | q == r       = Just 0
  | otherwise    = Nothing
unChurch' p q (App (Var r) s)
  | p == r       = fmap (+1) (unChurch' p q s)
  | otherwise    = Nothing
unChurch' p q _  = Nothing

isNum :: L -> Bool
isNum = maybe False (const True) . unChurch

showChurch :: L -> String
showChurch = showChurch' show

showChurch' :: (L -> String) -> L -> String
showChurch' f x = maybe (f x) show (unChurch x)
