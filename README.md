# chessCAMO

## Project Description

*chessCAMO* is a newly proposed chess variant that stands for **C**alculations **A**lways **M**ake **O**pportunities and was inspired by the very popular chess variant <a href="https://en.wikipedia.org/wiki/Crazyhouse" target="__blank">Crazyhouse</a>.

The "CAMO" portion comes from the idea that in this variant, at any turn, a player can sacrifice making a move to replace one of their pieces with a piece from the "piece reservoir". This has the affect of camouflaging each piece that is currently on the board. 

[![Build](https://img.shields.io/github/workflow/status/lbragile/chessCAMO/Unit_Testing/master)](https://github.com/lbragile/chessCAMO/actions) [![Coverage](https://img.shields.io/codecov/c/github/lbragile/chessCAMO/master)](https://codecov.io/gh/lbragile/chessCAMO/commits) [![Issues](https://img.shields.io/github/issues/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO/issues) [![Forks](https://img.shields.io/github/forks/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO) [![Stars](https://img.shields.io/github/stars/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO) [![Documentation](https://img.shields.io/badge/Doc-Available-brightgreen.svg)](https://lbragile.github.io/chessCAMO/) [![contributions welcome](https://img.shields.io/badge/Contributions-Welcome-brightgreen.svg?style=flat)](https://github.com/lbragile/chessCAMO) [![License](https://img.shields.io/github/license/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO/blob/master/LICENSE)

## Variant's Rules

1. The piece reservoir is limited in size and cannot be re-stocked with pieces.
2. Once a player decides to use a piece from the reservoir to replace one of his existing pieces on the board, their turn is over.
3. An obvious rule is that player cannot use the reservoir while in check as they must make a move to avoid check.
4. Additionally, just like in the *Crazyhouse* variant, a player cannot directly promote a pawn from the reservoir. That is a pawn from the reservoir must be placed on the board first and is only allowed to be promoted on the next move.

## To Do

#### <u>Regular Chess:</u>

- [ ] Three move repetition (draw) & 50 move rule.
- [ ] Board representation undo (use queue?).
- [ ] Input as PGN rather than two integers (template) - working currently but not really a template.
- [ ] GUI - nice to be able to move pieces with mouse rather than inputting coordinates.

#### <u>ChessCAMO:</u>

- [ ] Make piece reservoir (stack).
- [ ] Allow players to replace existing pieces with reservoir pieces.
- [ ] Implement abovementioned check and pawn promotion rules.
- [ ] Make test cases to confirm that overall functionality still works.

## Why?

I decided to create this project due to my growing interest in software development. Coming from an Engineering background with Deep Learning research experience (Recent Master of Applied Science graduate), I realized that writing and developing code (both front and back end) were highly
appealing to me. Thus, I spent a lot of time building upon my existing coding knowledge through online material and knew that it is time to test my knowledge on a challenging project. Additionally, I played chess (not very seriously) consistently since the age of about 4-5, so one can say that it is a very
big part of my life. As a result, this project allowed me to gain valuable experience and knowledge while working on a topic that I am passionate about and have expertise in.

## YouTube - Look Out

Be on the look out for a detailed YouTube video series explaining the various sections of this project. I feel like others could benefit from the logic involved in creating a project on this scale, which is relatively large and requires a substantial time commitment. Often times, when starting from scratch, an inexperienced software developer will quickly get overwhelmed by the complexity and decide to end the project before anything major is finished. I was in the same boat before and understand the frustration of wanting to work on an interesting project, but simply not being able to. It is often best to work on small pieces and build your way to the final product. This gives you a sense of accomplishment at each step and prevents you from experiencing the overwhelmed feeling. Let me know if you are interested in me making the videos and what you would like to see.