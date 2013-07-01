module L.Core 
{-
    ( L(..)
    , eval
    , apply
    , applyN
    , foldApp
    , importFunc
    , importLib
    )-} where

import Control.Monad (liftM2)
import Data.List (nub, intersect)

data L = Var String | Lam L L | App L L

instance Eq L where
    (==) = eq 0 []
      where
        -- Free variables must have the same string.
        -- Bound variables must have the same value in the environment.
        eq n env (Var x) (Var y) = x' == y' && maybe (x == y) id z
          where
            x' = lookup x env
            y' = lookup y env
            z = liftM2 (==) x' y'
        eq n env (Lam (Var x) y) (Lam (Var p) q) = eq (n+1) newEnv y q
          where newEnv = (x, n):(p, n):env
        eq n env (App x y) (App p q) = eq n env x p && eq n env y q
        eq _ _ _ _ = False

instance Show L where
    show (Var x)   = x
    show (Lam x y) = "(λ" ++ show x ++ "." ++ show y ++ ")"
    show (App x y) = "("  ++ show x ++ " " ++ show y ++ ")"

eval :: L -> L
eval x@(Var _) = x
eval (Lam x y) = Lam   (eval x) (eval y)
eval (App x y) = apply (eval x) (eval y)

apply :: L -> L -> L
apply (Lam x y) z = substitute x z (alpha z y)
apply x y = App x y

applyN :: [L] -> L
applyN = eval . foldApp

foldApp :: [L] -> L
foldApp (x:[]) = x
foldApp (x:xs) = foldl App x xs

importFunc :: String -> L -> L -> L
importFunc k v x = App (Lam (Var k) x) v

importLib :: L -> [(String, L)] -> L
importLib = foldr (uncurry importFunc)

substitute :: L -> L -> L -> L
substitute (Var k') v x@(Var x') = if x' == k' then v else x
substitute k@(Var k') v lam@(Lam x@(Var x') y)
  | x' == k'  = lam
  | otherwise = Lam x (substitute k v y)
substitute k v (App x y) = apply (substitute k v x) (substitute k v y)

-- rename y with respect to x
alpha :: L -> L -> L
alpha x y = foldr ($) y subs
  where
    collisions = nub $ boundVars x `intersect` boundVars y
    n = length collisions
    namespace = allVars x ++ allVars y
    subs = zipWith rename collisions (newVars n namespace)

rename :: String -> String -> L -> L
rename k v (Var x)   = if x == k then Var v else Var x
rename k v (Lam x y) = Lam (rename k v x) (rename k v y)
rename k v (App x y) = App (rename k v x) (rename k v y)

boundVars :: L -> [String]
boundVars = getVars False

allVars :: L -> [String]
allVars = getVars True

getVars :: Bool -> L -> [String]
getVars freeVars (Var x)  = if freeVars then [x] else []
getVars f (Lam (Var x) y) = [x]         ++ getVars f y
getVars f (App x y)       = getVars f x ++ getVars f y

newVars :: Int -> [String] -> [String]
newVars n xs = take n $ filter (not . flip elem xs) iterVars

-- a, b, ..., z, aa, ab, ..., az, ba, bb, ..., zz, aaa, aab, ...
iterVars :: [String]
iterVars = concatMap branch ("" : iterVars)
  where
    appendTo s = (s++) . return
    branch s = map (appendTo s) ['a'..'z']
