#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <cctype>
#include <memory>

Game::Game( ) :
    _player( LoadPlayerDefault( LoadAbilities( ) ) ),
    _playing( false )
{ }

void Game::Menu( )
{
    char input;

    while( true )
    {
        system( "CLS" );
        std::cout << "[1] Continue current game\n";
        std::cout << "[2] Load game from file\n";
        std::cout << "[3] Build new game (Randomization)\n";
        std::cout << "[4] Build new game (Configuration)\n";
        std::cout << "[5] Exit\n\n";
        input = GetChar( "Enter choice: ", { '1', '2', '3', '4', '5' } );
        _playing = true;

        switch( input )
        {
            case '1':
            {
                if( Exist( ) )
                {
                    Start( );
                }

                break;
            }
            case '2':
            {
                system( "CLS" );
                std::cout << "Loading, please wait.";

                try
                {
                    _config = LoadGameConfig( );
                    _dungeons = LoadGameDungeons( _player, _entityFactory, _index );
                }
                catch( const std::exception& error )
                {
                    std::cout << "\nError: " << error.what( );
                    std::cout << "\n\nPress enter to continue: ";
                    GetEnter( );
                }

                if( Exist( ) )
                {
                    Start( );
                }

                break;
            }
            case '3':
            {
                _config = DungeonConfiguration( );
                system( "CLS" );
                std::cout << "Loading, please wait.";
                Reset( );
                Start( );

                break;
            }
            case '4':
            {
                _config = GetDungeonConfiguration( );
                system( "CLS" );
                std::cout << "Loading, please wait.";
                Reset( );
                Start( );

                break;
            }
            case '5':
            {
                return;
            }
        }
    }
}
bool Game::Exist( ) const
{
    return _dungeons.size( ) != 0;
}

void Game::Reset( )
{
    _player.Reset( LoadPlayerDefault( LoadAbilities( ) ) );
    _dungeons.clear( );
    _dungeons.emplace_back( _player, _entityFactory, _config );
    _index = 0;
    DungeonLink( 0 );
    _dungeons[0].PlayerPlace( _dungeons[0].GetSize( ) / 2 );
}
void Game::Start( )
{
    while( _playing && _player.real->health > 0 )
    {
        NextTurn( _dungeons[_index] );
        _dungeons[_index].MovementRandom( );
        _dungeons[_index].Events( );
        _player.real->Update( );

        if( _player.real->health < 0 )
        {
            _playing = !_playing;
        }

        if( _player.real->states & States::Swapping )
        {
            _player.real->states &= ~States::Swapping;
            DungeonSwap( );
        }
    }
}
void Game::NextTurn( Dungeon& dungeon )
{
    static const std::map<char, Orientation::Enum> directions
    {
        { 'W', Orientation::North },
        { 'A', Orientation::West  },
        { 'S', Orientation::South },
        { 'D', Orientation::East  }
    };
    static const std::map<char, Orientation::Enum> rotations
    {
        { 'F', Orientation::East  },
        { 'G', Orientation::South },
        { 'H', Orientation::West  }
    };
    char input;

    AGAIN:
    system( "CLS" );
    PrintDungeon( dungeon, _player.real->visionReach, _player.real->position );
    PrintHealth( *_player.real );
    std::cout << "\n";
    std::cout << "[W] Go North\n";
    std::cout << "[A] Go West\n";
    std::cout << "[S] Go South\n";
    std::cout << "[D] Go East\n";
    std::cout << "[E] Exit to meny while saving\n";
    std::cout << "[R] Exit to meny without saving\n";
    std::cout << "[F] Rotate dungeon 90'\n";
    std::cout << "[G] Rotate dungeon 180'\n";
    std::cout << "[H] Rotate dungeon 270'\n\n";
    input = GetChar( "Enter choice: ", { 'W', 'A', 'S', 'D', 'E', 'R', 'F', 'G', 'H' }, std::toupper );

    switch( input )
    {
        case 'W':
        case 'A':
        case 'S':
        case 'D':
        {
            dungeon.MovementPlayer( directions.at( input ) );

            break;
        }
        case 'E':
        {
            _playing = !_playing;
            SaveGameConfig( _config );
            SaveGameDungeons( _dungeons, _index );

            break;
        }
        case 'R':
        {
            _playing = !_playing;

            break;
        }
        case 'F':
        case 'G':
        case 'H':
        {
            DungeonRotate( _index, rotations.at( input ) );

            goto AGAIN;
        }
    }
}
void Game::DungeonLink( int indexCurrentDungeon )
{
    const int limit = _dungeons[indexCurrentDungeon].links.size( );

    for( int indexCurrentLink = 0; indexCurrentLink < limit; indexCurrentLink++ )
    {
        if( _dungeons[indexCurrentDungeon].links[indexCurrentLink].indexLink < 0 &&
            _dungeons[indexCurrentDungeon].links[indexCurrentLink].indexDungeon < 0 )
        {
            _dungeons.emplace_back( _player, _entityFactory, _config );

            const int indexPartnerLink = 0;
            const int indexPartnerDungeon = _dungeons.size( ) - 1;
            Link& partner = _dungeons[indexPartnerDungeon].links[indexPartnerLink];
            Link& current = _dungeons[indexCurrentDungeon].links[indexCurrentLink];

            partner = { indexCurrentDungeon, indexCurrentLink, partner.entrance, current.entrance };
            current = { indexPartnerDungeon, indexPartnerLink, current.entrance, partner.entrance };
        }
    }
}
void Game::DungeonRotate( int indexDungeon, const Orientation::Enum& orientation )
{
    const Vector2<int> sizePrev = _dungeons[indexDungeon].GetSize( );

    for( auto& current : _dungeons[indexDungeon].links )
    {
        Link& partner = _dungeons[current.indexDungeon].links[current.indexLink];

        current.entrance = PositionRotate( current.entrance, sizePrev, orientation );
        partner.exit     = PositionRotate( partner.exit,     sizePrev, orientation );
    }

    _dungeons[indexDungeon].Rotate( orientation );
}
void Game::DungeonSwap( )
{
    const int limit = _dungeons[_index].links.size( );

    for( int i = 0; i < limit; i++ )
    {
        if( _dungeons[_index].links[i].entrance == _player.real->position )
        {
            const int indexPrev = _index;
            const int indexNext = _dungeons[_index].links[i].indexDungeon;
            const int entrance = _dungeons[indexPrev].GetQuadrant( _dungeons[indexPrev].links[i].entrance );
            const int exit = _dungeons[indexNext].GetQuadrant( _dungeons[indexPrev].links[i].exit );
            const int align = ( ( ( entrance - exit ) + 3 ) % 4 ) - 1;

            DungeonLink( indexNext );
            _index = indexNext;
            _dungeons[indexNext].PlayerPlace( _dungeons[indexPrev].links[i].exit );
            DungeonRotate( indexNext, static_cast<Orientation::Enum>( align ) );

            break;
        }
    }
}