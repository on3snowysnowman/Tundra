#!/bin/bash
set -e

# Paths
WORKTREE_DIR="../gh-pages-temp"
DOCS_DIR="docs/html"

echo "Removing old docs..."
rm -rf docs

echo "Generating Doxygen docs..."
doxygen Doxyfile

echo "Preparing gh-pages worktree..."
git worktree remove $WORKTREE_DIR --force 2>/dev/null || true
git worktree add $WORKTREE_DIR gh-pages

echo "Cleaning previous docs..."
rm -rf $WORKTREE_DIR/*

echo "Copying new docs..."
cp -r $DOCS_DIR/* $WORKTREE_DIR/

echo "Committing and pushing..."
cd $WORKTREE_DIR
git add .
git commit -m "Update Doxygen docs"
git push origin gh-pages
cd -

echo "Documentation deployed successfully!"
